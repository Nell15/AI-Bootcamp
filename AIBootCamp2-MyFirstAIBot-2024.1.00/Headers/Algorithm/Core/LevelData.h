#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include "Coordinates.h"
#include "Framework/InitData.h"

class LevelData
{
public:
    explicit LevelData(const SInitData& initData);
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

private:
	std::vector<Coordinates> goalTiles{};
	std::unordered_map<Coordinates, EHexCellType> tilesType{};
	std::unordered_map<Coordinates, std::vector<SObjectInfo>> objects{};

    void StoreTiles(const SInitData& initData);
    void StoreObjects(const SInitData& initData);
};


#endif
