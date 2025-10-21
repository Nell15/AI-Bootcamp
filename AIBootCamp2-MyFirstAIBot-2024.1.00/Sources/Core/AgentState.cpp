#include <utility>

#include "Core/AgentState.h"
#include "Core/Agent.h"
#include "Core/LevelData.h"
#include "PathFinding/PathFinder.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/ScoreSystem.h"
#include "Systems/TileSystem.h"

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
}

void Exploring::SetOrder(Agent& agent)
{
	// case door
	auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();
	
	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	for (auto& object : objectsAtPositon)
	{
		if (objectSystem.IsObjectAlreadyUsed(object)) continue;

		// todo : skip doors linked to pressure plates
		if (object.type == Door && object.state == Closed) {
			objectSystem.MarkUsed(object);

			SOrder order = {
				.orderType = static_cast<EOrderType>(Interact),
				.npcUID = agent.GetId(),
				.direction = object.direction,
				.objectUID = object.id,
				.interactionType = static_cast<EInteractionType>(OpenDoor)
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

	if (agent.IsPathEmpty() || not isNextMoveCorrect())
	{
		const Coordinates agentCoord = agent.GetPosition();
		const auto bestExploringPath = ScoreSystem::GetBestExploringPath(agentCoord);

		agent.SetPath(ConvertPathToOrder(agent, bestExploringPath));
	}
}

void Seeking::UpdateState(Agent& agent)
{
	const auto& tileSystem = Locator::Get<TileSystem>();
	const auto& goalTiles = tileSystem.GetGoalTiles();
	const Coordinates agentCoord = agent.GetPosition();

	if (goalTiles.empty())
	{
		agent.SetChosenGoal(std::nullopt);
		agent.SetState(make_unique<Exploring>());
	}
	else if (ranges::find(goalTiles, agentCoord) != goalTiles.end())
	{
		agent.SetState(make_unique<Waiting>());
	}
	else
	{
		PathFinder pathFinder{};

		for (const Coordinates& goalTile : goalTiles)
		{
			// TODO(opti): create a fonction DoGoalExist ?
			const auto path = pathFinder.FindPath(agent.GetPosition(), goalTile);
			if (path.has_value())
			{
				return;
			}
		}

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

	if (agent.IsPathEmpty() || not isNextMoveCorrect())
	{
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ? Avoid calculating path every turn

		size_t bestDistance = INT_MAX;
		vector<Coordinates> bestPath{};
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
					agent.SetChosenGoal(goalTile);
				}
			}
		}

		agent.SetPath(ConvertPathToOrder(agent, bestPath));
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

void SearchingHiddenDoors::SetOrder(Agent& agent) {
	// case wall
	auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();

	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	for (auto& object : objectsAtPositon)
	{
		if (objectSystem.WallWasAlreadyTested(object)) continue;

		if (object.type == Wall) {
			objectSystem.MarkUsed(object);

			SOrder order = {
				.orderType = static_cast<EOrderType>(Interact),
				.npcUID = agent.GetId(),
				.direction = object.direction,
				.objectUID = object.id,
				.interactionType = static_cast<EInteractionType>(SearchHiddenDoor)
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

	if (agent.IsPathEmpty() || not isNextMoveCorrect())
	{
		const Coordinates agentCoord = agent.GetPosition();
		const auto bestExploringPath = ScoreSystem::GetBestExploringPath(agentCoord); //Calcul du score à modifier pour qu'il utilise les murs non testés

		agent.SetPath(ConvertPathToOrder(agent, bestExploringPath));
	}
}

void SearchingHiddenDoors::UpdateState(Agent& agent) {
	auto& objSystem = Locator::Get<ObjectSystem>();
	auto objets = objSystem.GetInteractableObjectsAt(agent.GetPosition());

	auto it = std::find_if(objets.begin(), objets.end(), [](const auto& obj) {
		return obj.type == Door && obj.state == Closed;
		});

	if (it != objets.end()) {
		agent.SetState(make_unique<Exploring>());
	}
}
