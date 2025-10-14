#ifndef TILE_SYSTEM_H
#define TILE_SYSTEM_H

#include <unordered_map>

#include "AgentSystem.h"
#include "Core/Coordinates.h"
#include "Framework/Globals.h"

class TileSystem
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;

	[[nodiscard]] const GoalTilesType& GetGoalTiles() { return goalTiles; }
	[[nodiscard]] const TileArrayType& GetTiles() { return tiles; }
	[[nodiscard]] std::vector<Coordinates> GetAvailableGoalTiles();

	bool IsPossibleToWalkTo(Coordinates position, EHexCellDirection direction);
	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};

	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates position);
};

#endif
