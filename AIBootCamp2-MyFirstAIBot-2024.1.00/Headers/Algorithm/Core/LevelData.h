#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "Agent.h"
#include "Coordinates.h"
#include "Systems/AgentSystem.h"
#include "Systems/Locator.h"

class LevelData
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;

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

	[[nodiscard]] std::vector<Coordinates> GetAvailableGoalTiles()
	{
		const auto& agentSystem = Locator::Get<AgentSystem>();

		auto availableGoals = goalTiles
			| std::views::filter([&](const Coordinates goalPos)
			{
				return not agentSystem.IsTileOccupied(goalPos) and not agentSystem.IsGoalChosen(goalPos);
			});

		return {availableGoals.begin(), availableGoals.end()};
	}

	[[nodiscard]] const GoalTilesType& GetGoalTiles() { return goalTiles; }

	[[nodiscard]] const TileArrayType& GetTiles()
	{
		return tiles;
	}

	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);

	[[nodiscard]] std::vector<Coordinates> GetWalkableNeighbors(const Coordinates& tileCoord);
	[[nodiscard]] bool IsPossibleToWalkTo(Coordinates tileCoord, EHexCellDirection direction);
	[[nodiscard]] bool IsPossibleToWalkOnTile(const Coordinates& coord);
	[[nodiscard]] bool DoTileExist(const Coordinates& tileCoord) const;

	// TODO: put this in another class ?
	[[nodiscard]] Coordinates GetBestNeighbor(const Coordinates& tileCoord);

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};

	LevelData() = default;
	~LevelData() = default;
};


#endif
