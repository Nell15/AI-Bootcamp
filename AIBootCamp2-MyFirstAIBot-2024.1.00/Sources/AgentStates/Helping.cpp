#include "AgentStates/Helping.h"

#include "Core/Agent.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "Systems/ScoreSystem.h"
#include "AgentStates/Helping.h"
#include "AgentStates/Exploring.h"
#include "AgentStates/SearchingHiddenDoor.h"
#include "AgentStates/Seeking.h"
#include "PathFinding/PathFinder.h"

using namespace std;

void Helping::SetOrder(Agent& agent)
{
	// Stay still
	agent.AddOrder
	({
		.orderType = Move,
		.npcUID = agent.GetId(),
		.direction = CENTER
	});
}

void Helping::UpdateState(Agent& agent)
{
	const auto& agentSystem = Locator::Get<AgentSystem>();
	const auto& agents = agentSystem.GetAgents();
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& objectSystem = Locator::Get<ObjectSystem>();

	auto exitHelpingState = [&] {
		if (agent.HasNoOrders()) {
			agent.SetState(make_unique<Exploring>());
		}
		else {
			agent.SetState(make_unique<Seeking>());
		}
		};

	auto pressurePlate = objectSystem.GetPressurePlateAt(agent.GetPosition());
	if (!pressurePlate.has_value())
	{
		exitHelpingState();
		return;
	}
	vector<Object> connections = objectSystem.GetObjectConnections(pressurePlate.value());

	bool canMove = true;

	for (auto& [id, _agent] : agents) {
		if (id == agent.GetId() || _agent.GetStateName() == "Waiting") continue;

		const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();
		if (availableGoalTiles.empty()) break;

		PathFinder pathFinder{};

		for (const Coordinates& goalTile : availableGoalTiles) {
			const auto path = pathFinder.FindPath(agent.GetPosition(), goalTile);
			if (!path.has_value())
			{
				canMove = false;
				break;
			}

			auto previousTile = _agent.GetPosition();
			for (const auto coord : path.value()) {
				for (auto& object : connections) {
					Coordinates objTile = { object.q, object.r };
					if (objTile == coord
						&& object.direction == CoordUtils::CoordinatesToDir(previousTile - coord)) {
						canMove = false;
					}
					else if (objTile == previousTile
						&& object.direction == CoordUtils::CoordinatesToDir(coord - previousTile)) {
						canMove = false;
					}
				}
				previousTile = coord;
			}
		}
	}

	if (canMove) exitHelpingState();
}


