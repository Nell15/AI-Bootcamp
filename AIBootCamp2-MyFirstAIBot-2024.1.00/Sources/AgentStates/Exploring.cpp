#include "AgentStates/Exploring.h"

#include "Core/Agent.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "Systems/ScoreSystem.h"
#include "AgentStates/Helping.h"
#include "AgentStates/SearchingHiddenDoor.h"
#include "AgentStates/Seeking.h"
#include "PathFinding/PathFinder.h"

using namespace std;

void Exploring::UpdateState(Agent& agent)
{
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();
	const auto& objectSystem = Locator::Get<ObjectSystem>();

	const auto plate = objectSystem.GetPressurePlateAt(agent.GetPosition());
	if (plate.has_value() && !plate.value().connectionsIds.empty())
	{
		agent.SetState(make_unique<Helping>());
	}

	if (not availableGoalTiles.empty())
	{
		PathFinder pathFinder{};

		for (const Coordinates& goalTile : availableGoalTiles)
		{
			// TODO(opti): create a fonction DoGoalExist ?
			const auto path = pathFinder.FindPath(agent.GetPosition(), goalTile);
			if (path.has_value())
			{
				agent.SetChosenGoal(std::nullopt);
				agent.SetState(make_unique<Seeking>());
				break;
			}
		}
	}
	else
	{
		if (ScoreSystem::GetBestExploringPath(agent.GetPosition())[0] == agent.GetPosition()
			&& objectSystem.HasObject()
			&& objectSystem.GetNbClosedDoorOn(agent.GetPosition()) == 0)
		{
			agent.SetState(make_unique<SearchingHiddenDoors>());
		}
	}
}

void Exploring::SetOrder(Agent& agent)
{
	// case door
	const auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();

	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	for (auto& object : objectsAtPositon)
	{
		if (object.type == Door && object.state == Closed)
		{
			objectSystem.MarkUsed(object);

			const SOrder order = {
				.orderType = (Interact),
				.npcUID = agent.GetId(),
				.direction = object.direction,
				.objectUID = object.id,
				.interactionType = (OpenDoor)
			};

			agent.AddOrder(order);
			return;
		}
	}

	// case movement
	const auto isNextMoveCorrect = [&]
		{
			PathFinder pathFinder{};

			const Coordinates nextMove = agent.GetNextPosition();
			const auto path = pathFinder.FindPath(agent.GetPosition(), nextMove);

			return path.has_value() && path->back() == nextMove;
		};

	if (agent.HasNoOrders() || not isNextMoveCorrect())
	{
		const Coordinates agentCoord = agent.GetPosition();
		const auto bestExploringPath = ScoreSystem::GetBestExploringPath(agentCoord);

		agent.SetPath(AgentSystem::ConvertPathToOrder(agent, bestExploringPath));
	}
}