#include "Systems/ScoreSystem.h"

#include <queue>
#include <ranges>

#include "PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "Core/LevelData.h"

using namespace std;

int ScoreSystem::CalculateScore(const Coordinates position)
{
	int score = 0;

	for (const auto direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = position + direction;
		if (IsWorthToExplore(neighborPos) and CanExplore(position, direction))
			++score;
	}

	return score;
}

vector<Coordinates> ScoreSystem::GetBestExploringPath(const Coordinates position)
{
	const auto& tileSystem = Locator::Get<TileSystem>();

	priority_queue<TileScore, vector<TileScore>, MinScoreCompare> tileScores;
	for (const Coordinates& tile : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateScore(tile), tile);

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
	const Coordinates neighborPos = position + direction;

	return LevelData::Get().DoTileExist(neighborPos) and not objectSystem.IsPathBlocked(position, direction);
}
