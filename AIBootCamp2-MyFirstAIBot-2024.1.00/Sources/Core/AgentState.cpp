#include <utility>

#include "Core/AgentState.h"
#include "Core/Agent.h"
#include "Core/LevelData.h"
#include "PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/TileSystem.h"
#include "Utils/CoordUtils.h"
#include "Utils/Utils.h"

//termp

#include <fstream>
#include <iostream>



using namespace std;

namespace
{
	vector<SOrder> ConvertPathToOrder(const Agent& agent, const vector<Coordinates>& path)
	{
		vector<SOrder> npcOrders;

		npcOrders.resize(path.size());

		Coordinates currCoord = agent.GetPosition();

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
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();
	const auto& tiles = tileSystem.GetTiles();
	const auto& scoreSystem = Locator::Get<ScoreSystem>();
	const auto& objectSystem = Locator::Get<ObjectSystem>();
	
	auto plate = objectSystem.GetPressurePlateAt(agent.GetPosition());
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
	auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();

	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	for (auto& object : objectsAtPositon)
	{
		if (object.type == Door && object.state == Closed)
		{
			objectSystem.MarkUsed(object);

			SOrder order = {
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

		agent.SetPath(ConvertPathToOrder(agent, bestExploringPath));
	}
}

void Seeking::UpdateState(Agent& agent)
{
	const auto& objectSystem = Locator::Get<ObjectSystem>();

	auto plate = objectSystem.GetPressurePlateAt(agent.GetPosition());
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

	if (agent.HasNoOrders() || not isNextMoveCorrect())
	{
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
			agent.SetPath(ConvertPathToOrder(agent, bestPath));
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
	}
	else if (agentSystem.IsTileOccupied(agent.GetNextPosition()))
	{
		agent.AddOrder(
			{
				.orderType = Move,
				.npcUID = agent.GetId(),
				.direction = CENTER
			});
	}
}

void SearchingHiddenDoors::SetOrder(Agent& agent)
{
	auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();
	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	if (agent.HasNoOrders())
	{
		bool allWallsTested = true;
		for (auto& wall : objectsAtPositon)
		{
			if (wall.type == Wall && !objectSystem.WallWasAlreadyTested(wall))
			{
				allWallsTested = false;
				break;
			}
		}

		if (objectsAtPositon.empty() || allWallsTested) //Aucun mur à tester, on bouge
		{
			const Coordinates agentCoord = agent.GetPosition();
			const auto bestSearchingPath = ScoreSystem::GetBestSearchingPath(agentCoord);
			agent.SetPath(ConvertPathToOrder(agent, bestSearchingPath));
		}
		else //On test tous les murs sur la case
		{
			for (auto& object : objectsAtPositon)
			{
				if (objectSystem.WallWasAlreadyTested(object)) continue;
				//Ne devrait pas arriver, puisque c'est testé plus haut.

				if (object.type == Wall)
				{
					objectSystem.MarkUsed(object);

					SOrder order = {
						.orderType = (Interact),
						.npcUID = agent.GetId(),
						.direction = object.direction,
						.objectUID = object.id,
						.interactionType = (SearchHiddenDoor)
					};
					agent.AddOrder(order);
				}
			}
		}
	}
}

void SearchingHiddenDoors::UpdateState(Agent& agent)
{
	const auto& objSystem = Locator::Get<ObjectSystem>();
	auto objets = objSystem.GetInteractableObjectsAt(agent.GetPosition());

	const auto it = ranges::find_if(objets, [](const auto& obj)
	{
		return obj.type == Door && obj.state == Closed;
	});

	if (it != objets.end())
	{
		agent.SetState(make_unique<Exploring>());
	}
}

void Helping::UpdateState(Agent& agent)
{
	const auto& agentSystem = Locator::Get<AgentSystem>();
	const auto& agents = agentSystem.GetAgents();
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& objectSystem = Locator::Get<ObjectSystem>();

	auto optPressurePlate = objectSystem.GetPressurePlateAt(agent.GetPosition());
	if (!optPressurePlate.has_value()) return;
	auto pressurePlate = optPressurePlate.value();
	auto connectionIds = pressurePlate.connectionsIds;
	vector<Object> connections{};
	for (auto id : connectionIds) {
		auto object = objectSystem.GetObjectById(id);
		if (object.has_value()) connections.emplace_back(object.value());
	}
	
	bool canMove = true;
	bool pathFound = false;
	bool allWaiting = true;

	for (auto& [id, _agent] : agents) {
		if (id == agent.GetId() || _agent.GetStateName() == "Waiting") continue;
		allWaiting = false;

		const auto& availableGoalTiles = tileSystem.GetAvailableGoalTiles();
		if (availableGoalTiles.empty()) break;

		PathFinder pathFinder{};

		for (const Coordinates& goalTile : availableGoalTiles) {
			const auto path = pathFinder.FindPath(agent.GetPosition(), goalTile);
			if (!path.has_value()) continue;
			pathFound = true;

			auto previousTile = _agent.GetPosition();
			for (const auto coord : path.value()) {
				for (auto object : connections) {
					auto objTile = Coordinates(object.q, object.r);
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

	if ((canMove && pathFound) || allWaiting) {
		if (agent.HasNoOrders()) {
			agent.SetState(make_unique<Exploring>());
		}
		else {
			agent.SetState(make_unique<Seeking>());
		}
	}
}

void Helping::SetOrder(Agent& agent)
{
	// Stay still
	agent.AddOrder({
		.orderType = Move,
		.npcUID = agent.GetId(),
		.direction = CENTER
		});
}
