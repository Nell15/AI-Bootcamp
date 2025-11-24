#include "Systems/ScoreSystem.h"

#include <queue>
#include <ranges>

#include "PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"

using namespace std;

// Score for exploring state
float ScoreSystem::CalculateScore(const Coordinates position, const int distance)
{
	if (distance == 0)
		return 0.f; // don't score our own tile

	float baseScore = 0.f;
	for (const auto direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = position + direction;
		if ( IsWorthToExplore(neighborPos) && CanExplore(position, direction))
			baseScore += 1.f;
	}

	// Reward good exploration potential but penalize distant targets
	constexpr float distancePenalty = 0.3f; // tune this constant
	constexpr float frontierBonus = 1.2f; // if near unexplored area

	// Bonus if tile itself is unexplored
	if (IsWorthToExplore(position))
		baseScore *= frontierBonus;

	// Bonus if contains a pressure plate, weighted by distance to linked door(s?)
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	// check if has pressure plate :
	const auto optPlate = objectSystem.GetPressurePlateAt(position);
	const bool hasPlate = optPlate.has_value();
	// check connections && their distance
	//float connectionsBias = ? // bonus depending on the distance from the plate to its doors
	float connectionsBias = 0.0f;

	if (hasPlate)
	{
		// get plate
		auto& plate = optPlate.value();

		// get connections
		const std::vector<int> plateConnections = plate.connectionsIds;
		float minDistance = FLT_MAX;

		// for connection in connections
		for (const int id : plateConnections)
		{
			// get connected object
			auto connection = objectSystem.GetObjectById(id);
			
			if (!connection.has_value()) continue;
			// get distance to plate
			const int connDistance = CoordUtils::GetDistance(
				position,
				{.q = connection.value().q, .r = connection.value().r }
			);
			// update min distance
			if (distance < minDistance) minDistance = connDistance;
		}
		connectionsBias = (minDistance < FLT_MAX ? 10.0f / (1.0f + minDistance * minDistance) : 0.0f);
	}


	constexpr float plateWeight = 1.f; // Ajout arbitraire pour prioriser les plates
	const float plateScore = hasPlate * plateWeight + connectionsBias;

	// SCORE FOR TILES WITH DOORS
	const float doorScore = static_cast<float>(objectSystem.GetNbClosedDoorOn(position));

	// Penalize by distance (nonlinear decay works better than linear)
	const float score = (baseScore + plateScore + doorScore) / (1.f + distancePenalty * distance * distance);
	//const float score = (baseScore) / (1.f + distancePenalty * distance * distance);

	return score;
}

vector<Coordinates> ScoreSystem::GetBestExploringPath(const Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	// For each tile, store them in a priority queue by exploring score
	priority_queue<TileScore, vector<TileScore>, ScoreCompare> tileScores(ScoreCompare{position});
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScore(tile, CoordUtils::GetDistance(position, tile)), tile);

	// For each tile in order of score, we check whether there is a path to get there, and if there is one, we return it
	PathFinder pathFinder{};
	while (!tileScores.empty())
	{
		const Coordinates goal = tileScores.top().second;
		optional<vector<Coordinates>> path = pathFinder.FindPath(position, goal);
		if (path.has_value() and !path->empty() and tileScores.top().first > 0.f)
			return std::move(path.value());

		tileScores.pop();
	}

	// If there is no good tile to explore, return agent position
	return {position};
}

// Score for SearchingHiddenDoor state
float ScoreSystem::CalculateScoreByWalls(const Coordinates position, const int distance)
{
	const auto& objSystem = Locator::Get<ObjectSystem>();	

	float baseScore = 0.f;
	const auto& objects = objSystem.GetInteractableObjectsAt(position);
	for (auto& obj : objects)
		baseScore += static_cast<float>(obj.type == Wall && !objSystem.WallWasAlreadyTested(obj));

	return baseScore;
}

std::vector<Coordinates> ScoreSystem::GetBestSearchingPath(const Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	// For each tile, store them in a priority queue by "searching hidden door score"
	priority_queue<TileScore, vector<TileScore>, ScoreCompare> tileScores(ScoreCompare{ position });
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScoreByWalls(tile, CoordUtils::GetDistance(position, tile)), tile);

	// For each tile in order of score, we check whether there is a path to get there, and if there is one, we return it
	PathFinder pathFinder{};
	while (!tileScores.empty())
	{
		const Coordinates goal = tileScores.top().second;
		optional<vector<Coordinates>> path = pathFinder.FindPath(position, goal);
		if (path.has_value() and !path->empty() and tileScores.top().first > 0.f)
			return std::move(path.value());

		tileScores.pop();
	}
	return {};
	
}

// It is when the tile hasn't been explored yet
bool ScoreSystem::IsWorthToExplore(const Coordinates position)
{
	const auto& tiles = Locator::Get<TileSystem>().GetTiles();

	return not tiles.contains(position);
}

bool ScoreSystem::CanExplore(const Coordinates position, const EHexCellDirection direction)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const auto& tileSystem = Locator::Get<TileSystem>();
	const Coordinates neighborPos = position + direction;

	return tileSystem.DoTileExist(neighborPos) and not objectSystem.IsPathBlocked(position, direction);
}
