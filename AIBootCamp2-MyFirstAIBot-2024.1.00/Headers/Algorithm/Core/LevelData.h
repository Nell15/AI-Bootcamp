#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "Agent.h"
#include "Coordinates.h"

class LevelData
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;
	using ObjectArrayType = std::unordered_map<Coordinates, std::vector<SObjectInfo>>;
	LevelData(const LevelData&) = delete;
	LevelData& operator=(const LevelData&) = delete;

private:
	LevelData() = default;
	~LevelData() = default;

public:
	int rowCount{};
	int colCount{};
	int currentTurn{};

	static LevelData& Get() {
		static LevelData instance;
		return instance;
	}

	[[nodiscard]] int CalculateTileScore(const Coordinates& tileCoord);
	[[nodiscard]] std::vector<Coordinates> GetBestExploringTile(const Coordinates& tileCoord);

	[[nodiscard]] auto GetAvailableGoalTiles()
	{
		return goalTiles | std::views::filter([&](const Coordinates& tile)
			{
				return not IsTileOccupied(tile) && not IsGoalChosen(tile);
			});
	}

	[[nodiscard]] const GoalTilesType& GetGoalTiles() { return goalTiles; }

	[[nodiscard]] const TileArrayType& GetTiles()
	{
		return tiles;
	}

	[[nodiscard]] const ObjectArrayType& GetObjects()
	{
		return objects;
	}

	void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);
	void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

	void AddOccupiedTiles(const Coordinates& tileCoord) { occupiedTiles.emplace(tileCoord); }

	void UpdateOccupiedTile(const Coordinates& oldTile, const Coordinates& newTile)
	{
		occupiedTiles.erase(oldTile);
		occupiedTiles.emplace(newTile);
	}


	[[nodiscard]] bool IsTileOccupied(const Coordinates& tileCoord) const
	{
		return occupiedTiles.contains(tileCoord);
	}

	[[nodiscard]] std::vector<Coordinates> GetWalkableNeighbors(const Coordinates& tileCoord);
	[[nodiscard]] bool IsPossibleToWalkTo(const Coordinates& tileCoord, const Coordinates& directionCoord);
	[[nodiscard]] bool HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord);
	[[nodiscard]] bool IsPossibleToWalkOnTile(const Coordinates& coord);
	[[nodiscard]] bool DoTileExist(const Coordinates& tileCoord) const;

	// TODO: put this in another class ?
	[[nodiscard]] Coordinates GetBestNeighbor(const Coordinates& tileCoord);

	[[nodiscard]] std::vector<Agent>& GetAgents() { return agents; }

	

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
	ObjectArrayType objects{};

	std::vector<Agent> agents{};

	std::unordered_set<Coordinates> occupiedTiles{};

	[[nodiscard]] bool IsGoalChosen(const Coordinates& goal) const
	{
		for (const auto& agent : agents)
			if (agent.GetChosenGoal() == goal)
				return true;

		return false;
	}
};


#endif
