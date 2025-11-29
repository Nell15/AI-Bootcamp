#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include "Coordinates.h"

/**
* A class that represents information about the level being played.
  It remembers the row and column count as well as the current turn.
  It is mostly unused now, we divided it into 4 systems during the project.
*/
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
