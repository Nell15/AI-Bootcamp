#include <utility>

#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"
#include "Algorithm/Core/LevelData.h"
#include "Algorithm/PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/TileSystem.h"

using namespace std;

namespace
{
	vector<SOrder> ConvertPathToOrder(const Agent& agent, const vector<Coordinates>& path)
	{
		vector<SOrder> npcOrders;

		npcOrders.resize(path.size());

		Coordinates currCoord = agent.GetCoordinates();

		for (size_t i = 0; i < path.size(); ++i)
		{
			const Coordinates& nextCoord = path[path.size() - 1 - i];
			const SOrder order =
			{
				.orderType = Move,
				.npcUID = agent.GetId(),
				.direction = CoordUtils::GetNeighborDirection(currCoord, nextCoord)
			};

			npcOrders[path.size() - 1 - i] = order;
			currCoord = nextCoord;
		}

		return npcOrders;
	}
}

void Waiting::UpdateState(Agent& agent)
{
	// TODO: pour l'instant on fait rien
}

void Waiting::SetOrder(Agent& agent)
{
	agent.AddOrder({
		.orderType = Move,
		.npcUID = agent.GetId(),
		.direction = CENTER
	});
}

void Exploring::UpdateState(Agent& agent)
{
	auto& tileSystem = Locator::Get<TileSystem>();
	const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();

	if (not availableGoalTiles.empty())
	{
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ?

		for (const Coordinates& goalTile : availableGoalTiles)
		{
			// TODO(opti): create a fonction DoGoalExist ?
			const auto path = pathFinder.FindPath(agent.GetCoordinates(), goalTile);
			if (path.has_value())
			{
				agent.SetState(make_unique<Seeking>());
				break;
			}
		}
	}
}

void Exploring::SetOrder(Agent& agent)
{
	const Coordinates agentCoord = agent.GetCoordinates();
	const auto bestExploringPath = LevelData::Get().GetBestExploringTile(agentCoord);

	agent.SetPath(ConvertPathToOrder(agent, bestExploringPath));
}

void Seeking::UpdateState(Agent& agent)
{
	auto& tileSystem = Locator::Get<TileSystem>();
	const auto& goalTiles = tileSystem.GetGoalTiles();
	const Coordinates agentCoord = agent.GetCoordinates();

	if (goalTiles.empty())
	{
		agent.SetState(make_unique<Exploring>());
		agent.SetChosenGoal(std::nullopt);
	}
	else if (ranges::find(goalTiles, agentCoord) != goalTiles.end())
	{
		agent.SetState(make_unique<Waiting>());
	}
}

void Seeking::SetOrder(Agent& agent)
{
	const auto& agentSystem = Locator::Get<AgentSystem>();
	auto& tileSystem = Locator::Get<TileSystem>();
	const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();

	if (agent.IsPathEmpty())
	{
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ? Avoid calculating path every turn

		size_t bestDistance = INT_MAX;
		vector<Coordinates> bestPath{};
		for (const Coordinates& goalTile : availableGoalTiles)
		{
			const auto path = pathFinder.FindPath(agent.GetCoordinates(), goalTile);
			if (path.has_value())
			{
				const auto pathSize = path.value().size();
				if (cmp_less(pathSize, bestDistance))
				{
					bestDistance = pathSize;
					bestPath = path.value();
					agent.SetChosenGoal(goalTile);
				}
			}
		}

		agent.SetPath(ConvertPathToOrder(agent, bestPath));
	}
	else if (agentSystem.IsTileOccupied(agent.GetNextMove()))
	{
		agent.AddOrder(
			{
				.orderType = Move,
				.npcUID = agent.GetId(),
				.direction = CENTER
			});
	}
}
