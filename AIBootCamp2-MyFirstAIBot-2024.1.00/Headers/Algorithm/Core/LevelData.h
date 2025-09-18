#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include "Coordinates.h"

class LevelData
{
public:
    LevelData() = default;

	[[nodiscard]] const std::vector<Coordinates>& getGoalTiles() const
	{
        return goalTiles;
	}
	[[nodiscard]] const std::unordered_map<Coordinates, EHexCellType>& getTilesType() const
	{
        return tilesType;
	}
	[[nodiscard]] const std::unordered_map<Coordinates, std::vector<SObjectInfo>>& getObjects() const
	{
        return objects;
	}

	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);
	void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

private:
	std::vector<Coordinates> goalTiles{};
	std::unordered_map<Coordinates, EHexCellType> tilesType{};
	std::unordered_map<Coordinates, std::vector<SObjectInfo>> objects{};
};


#endif
