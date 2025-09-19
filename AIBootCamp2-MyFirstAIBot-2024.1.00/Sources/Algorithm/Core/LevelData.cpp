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

static bool IsPossibleToWalkOnTile(const Coordinates& coord, const LevelData::TileArrayType& tiles)
{
	const auto& tile = tiles.find(coord);

	return tile != tiles.end() && tile->second != Forbidden;
}

// TODO: check if not found
vector<Coordinates> LevelData::GetNeighbors(const Coordinates& tileCoord) const
{
	vector<Coordinates> neighbors{};

	if (const auto objectsOnTileIt = objects.find(tileCoord); objectsOnTileIt != objects.end())
	{
		for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
		{
			const Coordinates neighborPos = tileCoord + coordDir;
			const auto direction = Coordinates::CoordinatesToDir(coordDir);
			if (not Coordinates::AnyBlockingObjectInDirection(direction, objectsOnTileIt->second))
			{
				auto objectsOnNeighborTile = objects.find(neighborPos);
				if (objectsOnNeighborTile != objects.end())
				{
					const EHexCellDirection oppositeDirection = Coordinates::GetOppositeDirection(coordDir);
					if (not Coordinates::AnyBlockingObjectInDirection(oppositeDirection, objectsOnNeighborTile->second))
						if (IsPossibleToWalkOnTile(neighborPos, tiles))
							neighbors.emplace_back(neighborPos);
				}
				else if (IsPossibleToWalkOnTile(neighborPos, tiles))
					neighbors.emplace_back(neighborPos);
			}
		}
	}
	else
	{
		for (const Coordinates& coordDir : Coordinates::CoordinateDirections())
		{
			const Coordinates neighborPos = tileCoord + coordDir;

			auto objectsOnNeighborTile = objects.find(neighborPos);
			if (objectsOnNeighborTile != objects.end())
			{
				const EHexCellDirection oppositeDirection = tileCoord.GetOppositeDirection(coordDir);
				if (not Coordinates::AnyBlockingObjectInDirection(oppositeDirection, objectsOnNeighborTile->second))
					if (IsPossibleToWalkOnTile(neighborPos, tiles))
						neighbors.emplace_back(neighborPos);
			}
			else if (IsPossibleToWalkOnTile(neighborPos, tiles))
				neighbors.emplace_back(neighborPos);
		}
	}

	return neighbors;
}
