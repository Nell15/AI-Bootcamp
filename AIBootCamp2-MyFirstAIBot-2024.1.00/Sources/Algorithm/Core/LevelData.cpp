#include "Algorithm/Core/LevelData.h"

#include <array>
#include <span>
#include <algorithm>
#include <queue>

#include "Algorithm/PathFinding/PathFinder.h"
#include "Algorithm/Utils/CoordUtils.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"

using namespace std;

int LevelData::CalculateTileScore(const Coordinates& tileCoord) const
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	auto& tileSystem = Locator::Get<TileSystem>();
	int score = CoordUtils::NB_NEIGHBOR_DIRECTION;

	for (const auto direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = tileCoord + direction;
		if (tileSystem.GetTiles().contains(neighborPos) || not DoTileExist(neighborPos) || objectSystem.IsPathBlocked(tileCoord, direction))
			--score;
	}

	return score;
}

vector<Coordinates> LevelData::GetBestExploringTile(const Coordinates& tileCoord)
{
	using TileScore = std::pair<int, Coordinates>;
	struct MinScoreCompare
	{
		bool operator()(TileScore const& a, TileScore const& b) const
		{
			return a.first < b.first;
		}
	};

	auto& tileSystem = Locator::Get<TileSystem>();

	priority_queue<TileScore, vector<TileScore>, MinScoreCompare> tileScores;
	for (const auto key : tileSystem.GetTiles() | views::keys)
		tileScores.emplace(CalculateTileScore(key), key);

	PathFinder pathFinder{};
	optional<vector<Coordinates>> path;
	while (true)
	{
		const Coordinates goal = tileScores.top().second;
		path = pathFinder.FindPath(tileCoord, goal);
		if (path.has_value() && !path->empty())
			break;

		tileScores.pop();
	}

	return path.value();
}

Coordinates LevelData::GetBestNeighbor(const Coordinates& tileCoord)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const auto& agentSystem = Locator::Get<AgentSystem>();
	auto& tileSystem = Locator::Get<TileSystem>();

	Coordinates bestNeighbor = tileCoord;
	int bestNeighborScore = -1;

	for (const auto neighbor : GetWalkableNeighbors(tileCoord))
	{
		if (agentSystem.IsTileOccupied(neighbor))
			continue;

		int currentScore = CoordUtils::NB_NEIGHBOR_DIRECTION;

		for (const auto direction : CoordUtils::neighborDirection)
		{
			const Coordinates neighborPos = neighbor + direction;
			if (tileSystem.GetTiles().contains(neighborPos) or not DoTileExist(neighborPos) or objectSystem.IsPathBlocked(
				neighborPos, direction))
				--currentScore;
		}

		if (currentScore > bestNeighborScore)
		{
			bestNeighbor = neighbor;
			bestNeighborScore = currentScore;
		}
	}

	return bestNeighbor;
}

vector<Coordinates> LevelData::GetWalkableNeighbors(const Coordinates& tileCoord)
{
	auto& tileSystem = Locator::Get<TileSystem>();
	const auto& agentSystem = Locator::Get<AgentSystem>();
	vector<Coordinates> neighbors{};

	for (const auto& direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = tileCoord + direction;
		if (tileSystem.IsPossibleToWalkTo(tileCoord, direction) and not agentSystem.IsTileOccupied(neighborPos))
			neighbors.emplace_back(neighborPos);
	}

	return neighbors;
}

bool LevelData::DoTileExist(const Coordinates& tileCoord) const
{
	const int q = tileCoord.q;
	const int r = tileCoord.r;

	if (q < 0 || q >= rowCount) // 0 < q < rowCount
		return false;

	const int offset = q / 2;
	if (r < -offset || r >= colCount - offset) // 0 - (q / 2) < r < colCount - (q / 2)
		return false;

	return true;
}
