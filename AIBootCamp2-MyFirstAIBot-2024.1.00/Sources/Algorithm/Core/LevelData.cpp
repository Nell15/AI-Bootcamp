#include "Algorithm/Core/LevelData.h"

#include <array>
#include <span>
#include <algorithm>
#include <queue>

#include "Algorithm/PathFinding/PathFinder.h"
#include "Algorithm/Utils/CoordUtils.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"

using namespace std;

int LevelData::CalculateTileScore(const Coordinates& tileCoord) const
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	int score = CoordUtils::NB_NEIGHBOR_DIRECTION;

	for (const auto direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = tileCoord + direction;
		if (tiles.contains(neighborPos) || not DoTileExist(neighborPos) || objectSystem.IsPathBlocked(tileCoord, direction))
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

	priority_queue<TileScore, vector<TileScore>, MinScoreCompare> tileScores;
	for (const auto key : tiles | views::keys)
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

void LevelData::StoreTiles(const STileInfo* tileArrayInfo, const int nbTile)
{
	for (const auto tilesInfo = std::span{tileArrayInfo, static_cast<size_t>(nbTile)};
	     const auto& [q, r, type] : tilesInfo)
	{
		const Coordinates coords{.q = q, .r = r};
		tiles.try_emplace(coords, type);

		if (type == Goal && ranges::find(goalTiles, coords) == goalTiles.end())
			goalTiles.emplace_back(coords);
	}
}

Coordinates LevelData::GetBestNeighbor(const Coordinates& tileCoord)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	Coordinates bestNeighbor = tileCoord;
	int bestNeighborScore = -1;

	for (const auto neighbor : GetWalkableNeighbors(tileCoord))
	{
		if (IsTileOccupied(neighbor))
			continue;

		int currentScore = CoordUtils::NB_NEIGHBOR_DIRECTION;

		for (const auto direction : CoordUtils::neighborDirection)
		{
			const Coordinates neighborPos = neighbor + direction;
			if (tiles.contains(neighborPos) || not DoTileExist(neighborPos) || objectSystem.IsPathBlocked(
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
	vector<Coordinates> neighbors{};

	for (const auto& direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = tileCoord + direction;
		if (IsPossibleToWalkTo(tileCoord, direction) && not IsTileOccupied(neighborPos))
			neighbors.emplace_back(neighborPos);
	}

	return neighbors;
}

bool LevelData::IsPossibleToWalkTo(const Coordinates tileCoord, const EHexCellDirection direction)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const Coordinates neighborPos = tileCoord + direction;

	return IsPossibleToWalkOnTile(neighborPos) && not objectSystem.IsPathBlocked(tileCoord, direction);
}

bool LevelData::IsPossibleToWalkOnTile(const Coordinates& coord)
{
	const auto& tileIt = tiles.find(coord);

	return tileIt != tiles.end() && tileIt->second != Forbidden;
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
