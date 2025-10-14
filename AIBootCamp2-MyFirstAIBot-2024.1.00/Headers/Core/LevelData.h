#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include "Coordinates.h"

class LevelData
{
public:
	LevelData(const LevelData&) = delete;
	LevelData& operator=(const LevelData&) = delete;
	LevelData(LevelData&&) = delete;
	LevelData& operator=(LevelData&&) = delete;

	int rowCount{};
	int colCount{};
	int currentTurn{};

	static LevelData& Get()
	{
		static LevelData instance;
		return instance;
	}

	[[nodiscard]] bool DoTileExist(Coordinates tileCoord) const;

private:
	LevelData() = default;
	~LevelData() = default;
};


#endif
