#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <unordered_map>

#include "Coordinates.h"

class LevelData
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;
	using ObjectArrayType = std::unordered_map<Coordinates, std::vector<SObjectInfo>>;

    LevelData() = default;

	[[nodiscard]] const GoalTilesType& GetGoalTiles() const
	{
        return goalTiles;
	}
	[[nodiscard]] const TileArrayType& GetTiles() const
	{
        return tiles;
	}
	[[nodiscard]] const ObjectArrayType& GetObjects() const
	{
        return objects;
	}

	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);
	void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

	[[nodiscard]] std::vector<Coordinates> GetNeighbors(const Coordinates& tileCoord) const;

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
	ObjectArrayType objects{};
};


#endif
