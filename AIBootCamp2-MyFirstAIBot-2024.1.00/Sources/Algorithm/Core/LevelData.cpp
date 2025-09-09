#include "Algorithm/Core/LevelData.h"

#include <cassert>
#include <span>

using namespace std;

LevelData::LevelData(const SInitData& initData)
{
	StoreTiles(initData);

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
