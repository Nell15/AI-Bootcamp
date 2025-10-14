#include "Systems/TileSystem.h"

#include <ranges>
#include <span>

#include "Core/LevelData.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"

using namespace std;

void TileSystem::StoreTiles(const STileInfo* tileArrayInfo, const int nbTile)
{
	for (const auto tilesInfo = span{tileArrayInfo, static_cast<size_t>(nbTile)};
	     const auto& [q, r, type] : tilesInfo)
	{
		const Coordinates coords{.q = q, .r = r};
		tiles.try_emplace(coords, type);

		if (type == Goal and ranges::find(goalTiles, coords) == goalTiles.end())
			goalTiles.emplace_back(coords);
	}
}

void TileSystem::StoreNonExistingTiles()
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const auto& agents = Locator::Get<AgentSystem>().GetAgents();

	for (const Agent& agent : agents | views::values)
	{
		const Coordinates agentPos = agent.GetPosition();

		for (const EHexCellDirection direction : CoordUtils::neighborDirection)
		{
			const Coordinates neighborPos = agentPos + direction;

			if (not IsStored(neighborPos) and not objectSystem.IsPathBlocked(agentPos, direction))
				nonExistingTiles.emplace(neighborPos);
		}
	}
}

vector<Coordinates> TileSystem::GetAvailableGoalTiles() const
{
	const auto& agentSystem = Locator::Get<AgentSystem>();

	auto availableGoals = goalTiles
		| views::filter([&](const Coordinates goalPos)
		{
			return not agentSystem.IsTileOccupied(goalPos) and not agentSystem.IsGoalChosen(goalPos);
		});

	return {availableGoals.begin(), availableGoals.end()};
}

vector<Coordinates> TileSystem::GetWalkableNeighbors(const Coordinates position) const
{
	const auto& agentSystem = Locator::Get<AgentSystem>();
	vector<Coordinates> neighbors{};

	for (const EHexCellDirection direction : CoordUtils::neighborDirection)
	{
		const Coordinates neighborPos = position + direction;
		if (IsPossibleToWalkTo(position, direction) and not agentSystem.IsTileOccupied(neighborPos))
			neighbors.emplace_back(neighborPos);
	}

	return neighbors;
}

bool TileSystem::IsPossibleToWalkTo(const Coordinates position) const
{
	const auto& tileIt = tiles.find(position);

	return tileIt != tiles.end() and tileIt->second != Forbidden;
}

bool TileSystem::IsPossibleToWalkTo(const Coordinates position, const EHexCellDirection direction) const
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	const Coordinates neighborPos = position + direction;

	return IsPossibleToWalkTo(neighborPos) and not objectSystem.IsPathBlocked(position, direction);
}
