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

	int rowCount;
	int colCount;
	int currentTurn;

	[[nodiscard]] int CalculateTileScore(const Coordinates& tileCoord) const;
	[[nodiscard]] std::vector<Coordinates> GetBestExploringTile(const Coordinates& tileCoord) const;

	[[nodiscard]] auto GetAvailableGoalTiles() const
	{
		return goalTiles | std::views::filter([&](const Coordinates& tile)
			{
				return not IsTileOccupied(tile) && not IsGoalChosen(tile);
			});
	}

	[[nodiscard]] const GoalTilesType& GetGoalTiles() const { return goalTiles; }

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

	[[nodiscard]] std::vector<Coordinates> GetWalkableNeighbors(const Coordinates& tileCoord) const;
	[[nodiscard]] bool IsPossibleToWalkTo(const Coordinates& tileCoord, const Coordinates& directionCoord) const;
	[[nodiscard]] bool HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord) const;
	[[nodiscard]] bool IsPossibleToWalkOnTile(const Coordinates& coord) const;
	[[nodiscard]] bool DoTileExist(const Coordinates& tileCoord) const;

	// TODO: put this in another class ?
	[[nodiscard]] Coordinates GetBestNeighbor(const Coordinates& tileCoord) const;

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
