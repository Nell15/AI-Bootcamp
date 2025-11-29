#ifndef TILE_SYSTEM_H
#define TILE_SYSTEM_H

#include <unordered_map>

#include "AgentSystem.h"
#include "Core/Coordinates.h"
#include "Framework/Globals.h"

/**
* The system used to manage tiles.
*/
class TileSystem
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;

	[[nodiscard]] const TileArrayType& GetTiles() const { return tiles; }
	[[nodiscard]] const GoalTilesType& GetGoalTiles() const { return goalTiles; }
	[[nodiscard]] GoalTilesType GetAvailableGoalTiles() const;
	[[nodiscard]] std::vector<Coordinates> GetWalkableNeighbors(Coordinates position) const;
	[[nodiscard]] static std::vector<Coordinates> GetNeighbors(Coordinates position);

	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates position, EHexCellDirection direction) const;
	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates position) const;
	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);
	void StoreNonExistingTiles();

	[[nodiscard]] bool DoTileExist(const Coordinates position) const { return not nonExistingTiles.contains(position); }
	[[nodiscard]] bool IsStored(const Coordinates position) const { return tiles.contains(position); }

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
	std::unordered_set<Coordinates> nonExistingTiles;

};

#endif
