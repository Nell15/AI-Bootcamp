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

	[[nodiscard]] const TileArrayType& GetTiles() const { return tiles; }
	[[nodiscard]] const GoalTilesType& GetGoalTiles() const { return goalTiles; }
	[[nodiscard]] GoalTilesType GetAvailableGoalTiles() const;
	[[nodiscard]] std::vector<Coordinates> GetWalkableNeighbors(Coordinates position) const;

	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates position, EHexCellDirection direction) const;
	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};

	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates position) const;
};

#endif
