#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <algorithm>
#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "Agent.h"
#include "Coordinates.h"
#include "Object.h"

class LevelData
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;
	using ObjectArrayType = std::unordered_map<Coordinates, std::vector<Object>>;

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

	[[nodiscard]] int CalculateTileScore(const Coordinates& tileCoord);
	[[nodiscard]] std::vector<Coordinates> GetBestExploringTile(const Coordinates& tileCoord);

	[[nodiscard]] std::vector<Coordinates> GetAvailableGoalTiles()
	{
		auto availableGoals = goalTiles
			| std::views::filter([this](const Coordinates& coord)
			{
				return !IsTileOccupied(coord) && !IsGoalChosen(coord);
			});

		return {availableGoals.begin(), availableGoals.end()};
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

	static bool AnyBlockingObjectInDirection(EHexCellDirection direction, const std::vector<Object>& objectsOnTile)
	{
		const auto objectIt = std::ranges::find_if(objectsOnTile,
			[direction](const Object& obj)
			{
				return obj.direction == direction;
			});

		return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
	}

	static bool IsObstacle(const Object& object)
	{
		return object.type == Wall || object.type == Window;
	}


private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
	ObjectArrayType objects{};

	std::vector<Agent> agents{};
	std::unordered_set<Coordinates> occupiedTiles{};

	LevelData() = default;
	~LevelData() = default;

	[[nodiscard]] bool IsGoalChosen(const Coordinates& goal) const
	{
		return std::ranges::any_of(agents, [&](const auto& agent)
		{
			return agent.GetChosenGoal() == goal;
		});
	}
};


#endif
