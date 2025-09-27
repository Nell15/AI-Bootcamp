#ifndef LEVEL_DATA_H
#define LEVEL_DATA_H

#include <ranges>
#include <unordered_map>
#include <unordered_set>

#include "Coordinates.h"

class LevelData
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;
	using ObjectArrayType = std::unordered_map<Coordinates, std::vector<SObjectInfo>>;

	int qMax;
	int rMax;
	int currentTurn;

	[[nodiscard]] auto GetAvailableGoalTiles() const
	{
		return goalTiles | std::views::filter([&](const Coordinates& tile)
			{
				return not IsTileOccupied(tile);
			});
	}

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

	void ClearOccupiedTiles() { occupiedTiles.clear(); }
	void AddOccupiedTiles(const Coordinates& tileCoord) { occupiedTiles.emplace(tileCoord); }
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

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
	ObjectArrayType objects{};

	std::unordered_set<Coordinates> occupiedTiles{};
};


#endif
