#include "Algorithm/Core/LevelData.h"

#include <array>
#include <span>
#include <algorithm>

using namespace std;

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
		Coordinates objectCoordinates{object.q, object.r};
		auto& vec = objects[objectCoordinates];

		if (ranges::find(goalTiles, objectCoordinates) == goalTiles.end())
			vec.emplace_back(object);
	}
}

Coordinates LevelData::GetBestNeighbor(const Coordinates& tileCoord) const
{
	Coordinates bestNeighCoord;
	int bestNeighScore = -1;

	for (const auto neighborCoord : GetWalkableNeighbors(tileCoord))
	{
		int score = Coordinates::NB_COORDINATES;

		for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
		{
			const Coordinates neighborPos = tileCoord + coordDir;
			if (tiles.contains(neighborPos) || HasBlockingObject(tileCoord, neighborPos))
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

// TODO: check if not found
vector<Coordinates> LevelData::GetWalkableNeighbors(const Coordinates& tileCoord) const
{
	vector<Coordinates> neighbors{};

	for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
	{
		const Coordinates neighborPos = tileCoord + coordDir;
		if (IsPossibleToWalkTo(tileCoord, neighborPos))
			neighbors.emplace_back(neighborPos);
	}

	return neighbors;
}

bool LevelData::HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord) const
{
	const EHexCellDirection direction = Coordinates::CoordinatesToDir(directionCoord);
	const EHexCellDirection oppositeDirection = Coordinates::GetOppositeDirection(directionCoord);

	const auto objectsOnTileIt = objects.find(tileCoord);
	if (objectsOnTileIt == objects.end() || Coordinates::AnyBlockingObjectInDirection(direction, objectsOnTileIt->second))
		return false;

	const auto objectOnNeighborIt = objects.find(tileCoord);
	return objectOnNeighborIt != objects.end() && Coordinates::AnyBlockingObjectInDirection(oppositeDirection, objectOnNeighborIt->second);
}

bool LevelData::IsPossibleToWalkTo(const Coordinates& tileCoord, const Coordinates& directionCoord) const
{
	const Coordinates neighborPos = tileCoord + directionCoord;

	return IsPossibleToWalkOnTile(neighborPos) && not HasBlockingObject(neighborPos, directionCoord);
}

bool LevelData::IsPossibleToWalkOnTile(const Coordinates& coord) const
{
	const auto& tileIt = tiles.find(coord);

	return tileIt != tiles.end() && tileIt->second != Forbidden;
}
