#include "Algorithm/Core/LevelData.h"

#include <span>
#include <algorithm>

using namespace std;

void LevelData::StoreTiles(const STileInfo* tileArrayInfo, const int nbTile)
{
	for (const auto tiles = std::span{tileArrayInfo, static_cast<size_t>(nbTile)}; const
	     auto& [q, r, type] : tiles)
	{
		const Coordinates coords{.q = q, .r = r};
		tilesType.try_emplace(coords, type);

		if (type == Goal && ranges::find(goalTiles, coords) == goalTiles.end())
			goalTiles.emplace_back(coords);
	}
}

void LevelData::StoreObjects(const SObjectInfo* objectArrayInfo, const int nbObject)
{
	const span objectInfos{ objectArrayInfo, static_cast<size_t>(nbObject) };

	for (const auto& object : objectInfos)
	{
		Coordinates objectCoordinates{ object.q, object.r };
		auto& vec = objects[objectCoordinates];

		if (ranges::find(goalTiles, objectCoordinates) == goalTiles.end())
			vec.emplace_back(object);
	}
}
