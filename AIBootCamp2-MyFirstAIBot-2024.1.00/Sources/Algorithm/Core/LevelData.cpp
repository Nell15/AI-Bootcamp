#include "Algorithm/Core/LevelData.h"

#include <array>
#include <span>
#include <algorithm>
#include <queue>

#include "Algorithm/PathFinding/PathFinder.h"

using namespace std;

int LevelData::CalculateTileScore(const Coordinates& tileCoord) const
{
	int score = Coordinates::NB_COORDINATES;

	for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
	{
		const Coordinates neighborPos = tileCoord + coordDir;
		if (tiles.contains(neighborPos) || not DoTileExist(neighborPos) || HasBlockingObject(tileCoord, coordDir))
			--score;
	}

	return score;
}

vector<Coordinates> LevelData::GetBestExploringTile(const Coordinates& tileCoord) const
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

	PathFinder pathFinder{ *this };
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

void LevelData::StoreObjects(const SObjectInfo* objectArrayInfo, const int nbObject)
{
	const span objectInfos{objectArrayInfo, static_cast<size_t>(nbObject)};

	for (const auto& object : objectInfos)
	{
		Coordinates objectCoordinates{.q = object.q, .r = object.r};
		auto& vec = objects[objectCoordinates];

		if (ranges::find(vec, object) == vec.end())
			vec.emplace_back(object);
	}
}

Coordinates LevelData::GetBestNeighbor(const Coordinates& tileCoord) const
{
	Coordinates bestNeighCoord = tileCoord;
	int bestNeighScore = -1;

	for (const auto neighborCoord : GetWalkableNeighbors(tileCoord))
	{
		if (IsTileOccupied(neighborCoord))
			continue;

		int score = Coordinates::NB_COORDINATES;

		for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
		{
			const Coordinates neighborPos = neighborCoord + coordDir;
			if (tiles.contains(neighborPos) || not DoTileExist(neighborPos) || HasBlockingObject(
				neighborCoord, coordDir))
				--score;
		}

		if (score > bestNeighScore)
		{
			bestNeighCoord = neighborCoord;
			bestNeighScore = score;
		}
	}

	return bestNeighCoord;
}

vector<Coordinates> LevelData::GetWalkableNeighbors(const Coordinates& tileCoord) const
{
	vector<Coordinates> neighbors{};

	for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
	{
		const Coordinates neighborPos = tileCoord + coordDir;
		if (IsPossibleToWalkTo(tileCoord, coordDir))
			neighbors.emplace_back(neighborPos);
	}

	return neighbors;
}

bool LevelData::HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord) const
{
	const Coordinates neighborPos = tileCoord + directionCoord;
	const EHexCellDirection direction = Coordinates::CoordinatesToDir(directionCoord);
	const EHexCellDirection oppositeDirection = Coordinates::GetOppositeDirection(directionCoord);

	const auto objectsOnTileIt = objects.find(tileCoord);
	if (objectsOnTileIt != objects.end() && Coordinates::AnyBlockingObjectInDirection(
		direction, objectsOnTileIt->second))
		return true;

	const auto objectOnNeighborIt = objects.find(neighborPos);
	return objectOnNeighborIt != objects.end() && Coordinates::AnyBlockingObjectInDirection(
		oppositeDirection, objectOnNeighborIt->second);
}

bool LevelData::IsPossibleToWalkTo(const Coordinates& tileCoord, const Coordinates& directionCoord) const
{
	const Coordinates neighborPos = tileCoord + directionCoord;

	return IsPossibleToWalkOnTile(neighborPos) && not HasBlockingObject(tileCoord, directionCoord);
}

bool LevelData::IsPossibleToWalkOnTile(const Coordinates& coord) const
{
	const auto& tileIt = tiles.find(coord);

	return tileIt != tiles.end() && tileIt->second != Forbidden;
}

bool LevelData::DoTileExist(const Coordinates& tileCoord) const
{
	const int q = tileCoord.q;
	const int r = tileCoord.r;

	if (q < 0 || q >= qMax) // 0 < q < qMax
		return false;

	const int offset = q / 2;
	if (r < -offset || r >= rMax - offset) // 0 - (q / 2) < r < rMax - (q / 2)
		return false;

	return true;
}
