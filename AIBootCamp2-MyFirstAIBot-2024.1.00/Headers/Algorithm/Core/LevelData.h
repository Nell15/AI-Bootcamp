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

	inline static int rowCount{};
	inline static int colCount{};
	inline static int currentTurn{};

	[[nodiscard]] static int CalculateTileScore(const Coordinates& tileCoord);
	[[nodiscard]] static std::vector<Coordinates> GetBestExploringTile(const Coordinates& tileCoord);

	[[nodiscard]] static auto GetAvailableGoalTiles()
	{
		return goalTiles | std::views::filter([&](const Coordinates& tile)
			{
				return not IsTileOccupied(tile) && not IsGoalChosen(tile);
			});
	}

	[[nodiscard]] static const GoalTilesType& GetGoalTiles() { return goalTiles; }

	[[nodiscard]] static const TileArrayType& GetTiles()
	{
		return tiles;
	}

	[[nodiscard]] static const ObjectArrayType& GetObjects()
	{
		return objects;
	}

	static void StoreTiles(const STileInfo* tileArrayInfo, int nbTile);
	static void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

	static void AddOccupiedTiles(const Coordinates& tileCoord) { occupiedTiles.emplace(tileCoord); }

	static void UpdateOccupiedTile(const Coordinates& oldTile, const Coordinates& newTile)
	{
		occupiedTiles.erase(oldTile);
		occupiedTiles.emplace(newTile);
	}


	[[nodiscard]] static bool IsTileOccupied(const Coordinates& tileCoord)
	{
		return occupiedTiles.contains(tileCoord);
	}

	[[nodiscard]] static std::vector<Coordinates> GetWalkableNeighbors(const Coordinates& tileCoord);
	[[nodiscard]] static bool IsPossibleToWalkTo(const Coordinates& tileCoord, const Coordinates& directionCoord);
	[[nodiscard]] static bool HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord);
	[[nodiscard]] static bool IsPossibleToWalkOnTile(const Coordinates& coord);
	[[nodiscard]] static bool DoTileExist(const Coordinates& tileCoord);

	// TODO: put this in another class ?
	static [[nodiscard]] Coordinates GetBestNeighbor(const Coordinates& tileCoord);

	static [[nodiscard]] std::vector<Agent>& GetAgents() { return agents; }

	

private:
	inline static GoalTilesType goalTiles{};
	inline static TileArrayType tiles{};
	inline static ObjectArrayType objects{};

	inline static std::vector<Agent> agents{};

	inline static std::unordered_set<Coordinates> occupiedTiles{};

	static [[nodiscard]] bool IsGoalChosen(const Coordinates& goal)
	{
		for (const auto& agent : agents)
			if (agent.GetChosenGoal() == goal)
				return true;

		return false;
	}
};


#endif
