#ifndef TILE_SYSTEM_H
#define TILE_SYSTEM_H

#include <ranges>
#include <unordered_map>

#include "AgentSystem.h"
#include "Locator.h"
#include "ObjectSystem.h"
#include "Algorithm/Core/Coordinates.h"
#include "Framework/Globals.h"

class TileSystem
{
public:
	using GoalTilesType = std::vector<Coordinates>;
	using TileArrayType = std::unordered_map<Coordinates, EHexCellType>;

	[[nodiscard]] const GoalTilesType& GetGoalTiles() { return goalTiles; }
	[[nodiscard]] const TileArrayType& GetTiles() { return tiles; }

	void StoreTiles(const STileInfo* tileArrayInfo, const int nbTile)
	{
		for (const auto tilesInfo = std::span{ tileArrayInfo, static_cast<size_t>(nbTile) };
			const auto& [q, r, type] : tilesInfo)
		{
			const Coordinates coords{ .q = q, .r = r };
			tiles.try_emplace(coords, type);

			if (type == Goal && std::ranges::find(goalTiles, coords) == goalTiles.end())
				goalTiles.emplace_back(coords);
		}
	}

	[[nodiscard]] std::vector<Coordinates> GetAvailableGoalTiles()
	{
		const auto& agentSystem = Locator::Get<AgentSystem>();

		auto availableGoals = goalTiles
			| std::views::filter([&](const Coordinates goalPos)
				{
					return not agentSystem.IsTileOccupied(goalPos) and not agentSystem.IsGoalChosen(goalPos);
				});

		return { availableGoals.begin(), availableGoals.end() };
	}

	[[nodiscard]] bool IsPossibleToWalkTo(const Coordinates& position)
	{
		const auto& tileIt = tiles.find(position);

		return tileIt != tiles.end() && tileIt->second != Forbidden;
	}


	bool IsPossibleToWalkTo(const Coordinates tileCoord, const EHexCellDirection direction)
	{
		const auto& objectSystem = Locator::Get<ObjectSystem>();
		const Coordinates neighborPos = tileCoord + direction;

		return IsPossibleToWalkTo(neighborPos) && not objectSystem.IsPathBlocked(tileCoord, direction);
	}

private:
	GoalTilesType goalTiles{};
	TileArrayType tiles{};
};

#endif
