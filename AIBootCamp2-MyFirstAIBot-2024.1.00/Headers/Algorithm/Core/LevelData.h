#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include "Coordinates.h"
#include "Systems/AgentSystem.h"

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

	[[nodiscard]] int CalculateTileScore(const Coordinates& tileCoord) const;
	[[nodiscard]] std::vector<Coordinates> GetBestExploringTile(const Coordinates& tileCoord);

	

	[[nodiscard]] static std::vector<Coordinates> GetWalkableNeighbors(const Coordinates& tileCoord);
	[[nodiscard]] bool DoTileExist(const Coordinates& tileCoord) const;

	// TODO: put this in another class ?
	[[nodiscard]] Coordinates GetBestNeighbor(const Coordinates& tileCoord);

private:
	LevelData() = default;
	~LevelData() = default;
};


#endif
