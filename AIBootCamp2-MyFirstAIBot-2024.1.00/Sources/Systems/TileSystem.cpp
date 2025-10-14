#include "Systems/TileSystem.h"

#include <ranges>
#include <span>

#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"

using namespace std;

void TileSystem::StoreTiles(const STileInfo* tileArrayInfo, const int nbTile)
{
	for (const auto tilesInfo = span{ tileArrayInfo, static_cast<size_t>(nbTile) };
		const auto& [q, r, type] : tilesInfo)
	{
		const Coordinates coords{ .q = q, .r = r };
		tiles.try_emplace(coords, type);

		if (type == Goal && ranges::find(goalTiles, coords) == goalTiles.end())
			goalTiles.emplace_back(coords);
	}
}

vector<Coordinates> TileSystem::GetAvailableGoalTiles()
{
	const auto& agentSystem = Locator::Get<AgentSystem>();

	auto availableGoals = goalTiles
		| views::filter([&](const Coordinates goalPos)
			{
				return not agentSystem.IsTileOccupied(goalPos) and not agentSystem.IsGoalChosen(goalPos);
			});

	return { availableGoals.begin(), availableGoals.end() };
}

bool TileSystem::IsPossibleToWalkTo(const Coordinates position)
{
	const auto& tileIt = tiles.find(position);

	return tileIt != tiles.end() && tileIt->second != Forbidden;
}

bool TileSystem::IsPossibleToWalkTo(const Coordinates position, const EHexCellDirection direction)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const Coordinates neighborPos = position + direction;

	return IsPossibleToWalkTo(neighborPos) && not objectSystem.IsPathBlocked(position, direction);
}
