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
		if (IsWorthToExplore(neighborPos) && CanExplore(position, direction))
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
	bool hasPlate = objectSystem.HasPressurePlateAt(position);
	// check connections && their distance
	float plateWeight = 3.0f; // TODO
	float plateBias = hasPlate * plateWeight;

	// Penalize by distance (nonlinear decay works better than linear)
	const float score = (baseScore + plateBias) / (1.f + distancePenalty * distance * distance);

	return score;
}

vector<Coordinates> ScoreSystem::GetBestExploringPath(const Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	priority_queue<TileScore, vector<TileScore>, ScoreCompare> tileScores(ScoreCompare{position});
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScore(tile, CoordUtils::GetDistance(position, tile)), tile);

	PathFinder pathFinder{};
	optional<vector<Coordinates>> path;
	while (true)
	{
		const Coordinates goal = tileScores.top().second;
		path = pathFinder.FindPath(position, goal);
		if (path.has_value() and !path->empty())
			break;

		tileScores.pop();
	}

	return std::move(path.value());
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
