#include "AgentStates/Seeking.h"

#include <algorithm>
#include <ranges>

#include "Core/Agent.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "AgentStates/Exploring.h"
#include "AgentStates/Helping.h"
#include "AgentStates/Waiting.h"
#include "PathFinding/PathFinder.h"

using namespace std;

void Seeking::UpdateState(Agent& agent)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();

	const auto plate = objectSystem.GetPressurePlateAt(agent.GetPosition());
	if (plate.has_value() && !plate.value().connectionsIds.empty())
	{
		agent.SetState(make_unique<Helping>());
	}

	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& goalTiles = tileSystem.GetGoalTiles();
	const Coordinates agentCoord = agent.GetPosition();

	if (ranges::find(goalTiles, agentCoord) != goalTiles.end())
	{
		agent.SetChosenGoal(std::nullopt);
		agent.SetState(make_unique<Waiting>());
	}
	else if (goalTiles.empty())
	{
		agent.SetChosenGoal(std::nullopt);
		agent.SetState(make_unique<Exploring>());
	}
}

void Seeking::SetOrder(Agent& agent)
{
	const auto& agentSystem = Locator::Get<AgentSystem>();
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();

	const auto isNextMoveCorrect = [&]
		{
			PathFinder pathFinder{};

			const Coordinates nextMove = agent.GetNextPosition();
			const auto path = pathFinder.FindPath(agent.GetPosition(), nextMove);

			return path.has_value() && path->back() == nextMove;
		};

	auto changePathOrReturnToExplore = [&] {
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ? Avoid calculating orders every turn

		size_t bestDistance = INT_MAX;
		vector<Coordinates> bestPath{};
		Coordinates bestGoal{};
		for (const Coordinates& goalTile : availableGoalTiles)
		{
			const auto path = pathFinder.FindPath(agent.GetPosition(), goalTile);
			if (path.has_value())
			{
				const auto pathSize = path.value().size();
				if (cmp_less(pathSize, bestDistance))
				{
					bestDistance = pathSize;
					bestPath = path.value();
					bestGoal = goalTile;
				}
			}
		}

		// Si on a trouvé un chemin, on l'utilise
		if (!bestPath.empty())
		{
			agent.SetChosenGoal(bestGoal);
			agent.SetPath(AgentSystem::ConvertPathToOrder(agent, bestPath));
		}
		// Sinon on explore
		else
		{
			agent.AddOrder({
				.orderType = Move,
				.npcUID = agent.GetId(),
				.direction = CENTER
				});
			agent.SetChosenGoal(std::nullopt);
			agent.SetState(make_unique<Exploring>());
		}
		};

	if (agent.HasNoOrders() || not isNextMoveCorrect())
	{
		changePathOrReturnToExplore();
	}
	else if (const auto nextPos = agent.GetNextPosition(); agentSystem.IsTileOccupied(nextPos))
	{
		// If the other guy is not waiting (ie: will move)
		if (const auto otherAgent = agentSystem.GetAgentAt(nextPos);
			otherAgent.has_value() && otherAgent->get().GetStateName() != "Waiting")
		{
			agent.AddOrder(
				{
					.orderType = Move,
					.npcUID = agent.GetId(),
					.direction = CENTER
				});
		}
		else
		{
			changePathOrReturnToExplore();
		}
	}
}