#ifndef TILE_SYSTEM_H
#define TILE_SYSTEM_H

#include <unordered_map>

#include "Algorithm/Core/Coordinates.h"
#include "Framework/Globals.h"

class TileSystem
{
public:
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;

private:
	TileArrayType tiles{};
};

#endif