#include "Algorithm/Core/LevelData.h"

#include <cassert>
#include <span>
#include <algorithm>

using namespace std;

LevelData::LevelData(const SInitData& initData)
{
	StoreTiles(initData);
	StoreObjects(initData);

	assert(!tilesType.empty() && "Tiles are missing");
	assert(!goalTiles.empty() && "Goal tiles are missing");
}

void LevelData::StoreTiles(const SInitData& initData)
{
	tilesType.reserve(initData.tileInfoArraySize);
	goalTiles.reserve(initData.tileInfoArraySize);

	for (const auto tiles = std::span{initData.tileInfoArray, static_cast<size_t>(initData.tileInfoArraySize)}; const
	     auto& [q, r, type] : tiles)
	{
		const Coordinates coords{.q = q, .r = r};
		tilesType.emplace(coords, type);

		if (type == Goal)
			goalTiles.emplace_back(coords);
	}

	goalTiles.shrink_to_fit();
}

void LevelData::StoreObjects(const SInitData& initData)
{
	objects.reserve(initData.objectInfoArraySize);
	
	for (const auto objectInfos = std::span{ initData.objectInfoArray, static_cast<size_t>(initData.objectInfoArraySize) }; const
		SObjectInfo& objectInfo : objectInfos)
	{
		const Coordinates objectCoordinates = { objectInfo.q, objectInfo.r };
		auto objectIt = objects.find(objectCoordinates);

		if (objectIt != objects.end())
		{
			objectIt->second.emplace_back(objectInfo);
		}

		objects.emplace(objectCoordinates, vector{ objectInfo });
	}
}
