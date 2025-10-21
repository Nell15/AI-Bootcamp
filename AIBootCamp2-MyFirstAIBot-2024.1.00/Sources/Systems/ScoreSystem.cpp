#include "Systems/ScoreSystem.h"

#include <queue>
#include <ranges>

#include "PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"

using namespace std;

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
	constexpr float distancePenalty = 0.3f;   // tune this constant
	constexpr float frontierBonus = 1.2f;   // if near unexplored area

	// Bonus if tile itself is unexplored (encourages frontier expansion)
	if (IsWorthToExplore(position))
		baseScore *= frontierBonus;

	// Bonus if contains a pressure plate, weighted by distance to linked door(s?)
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	// check if has pressure plate :
	auto optPlate = objectSystem.GetPressurePlateAt(position);
	bool hasPlate = optPlate.has_value();
	// check connections && their distance
	//float connectionsBias = ? // bonus depending on the distance from the plate to its doors
	float connectionsBias = 0.0f;

	if (hasPlate)
	{
		// get plate
		auto& plate = optPlate.value();

		// get connections
		std::vector<int> plateConnections = plate.connectionsIds;
		float minDistance = FLT_MAX;

		// for connection in connections
		for (int id : plateConnections)
		{
			// get connected object
			auto connection = objectSystem.GetObjectById(id);
			
			if (!connection.has_value()) continue;
			// get distance to plate
			int connDistance = CoordUtils::GetDistance(
				position,
				{ connection.value().q, connection.value().r }
			);
			// update min distance
			if (distance < minDistance) minDistance = connDistance;
		}
		//connectionsBias = (minDistance < FLT_MAX ? 10.0f / (1.0f + minDistance * minDistance) : 0.0f); //Inutilisé, car trop d'influence et brise l'algo
	}
	
	float plateWeight = 1.f; // Ajout arbitraire pour prioriser les plates
	float plateScore = hasPlate * plateWeight;// + connectionsBias; //Inutilisé, car trop d'influence et brise l'algo
	
	// Penalize by distance (nonlinear decay works better than linear)
	const float score = (baseScore + plateScore) / (1.f + distancePenalty * distance * distance);
	//const float score = (baseScore) / (1.f + distancePenalty * distance * distance);

	return score;
}

vector<Coordinates> ScoreSystem::GetBestExploringPath(const Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	priority_queue<TileScore, vector<TileScore>, ScoreCompare> tileScores(ScoreCompare{position});
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScore(tile, CoordUtils::GetDistance(position, tile)), tile);

	PathFinder pathFinder{};
	while (!tileScores.empty())
	{
		const Coordinates goal = tileScores.top().second;
		optional<vector<Coordinates>> path = pathFinder.FindPath(position, goal);
		if (path.has_value() and !path->empty() and tileScores.top().first > 0.f)
			return std::move(path.value());

		tileScores.pop();
	}
	return {position};
	
}

float ScoreSystem::CalculateScoreByWalls(const Coordinates position, const int distance) {
	auto& objSystem = Locator::Get<ObjectSystem>();	

	float baseScore = 0.f;
	if (distance == 0) {
		const auto& objets = objSystem.GetInteractableObjectsAt(position);

		for (auto& obj : objets) {
			baseScore += static_cast<float>(obj.type == Wall && !objSystem.WallWasAlreadyTested(obj));
		}
		return baseScore;
	}

	
	for (const auto direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = position + direction;
		const auto& objets = objSystem.GetInteractableObjectsAt(neighborPos);
		for (auto& obj : objets) {
			baseScore += static_cast<float>(obj.type == Wall && !objSystem.WallWasAlreadyTested(obj));
		}			
	}

	return baseScore;
}

std::vector<Coordinates> ScoreSystem::GetBestSearchingPath(Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	priority_queue<TileScore, vector<TileScore>, ScoreCompare> tileScores(ScoreCompare{ position });
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScoreByWalls(tile, CoordUtils::GetDistance(position, tile)), tile);//On assume que c'est parfait :)

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
