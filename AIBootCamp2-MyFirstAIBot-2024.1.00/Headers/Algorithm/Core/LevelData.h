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

private:
	std::vector<Coordinates> goalTiles{};
	std::unordered_map<Coordinates, EHexCellType> tilesType{};

    void StoreTiles(const SInitData& initData);
};


#endif
