#include "AgentStates/SearchingHiddenDoor.h"

#include "Core/Agent.h"
#include "Systems/Locator.h"
#include "Systems/ObjectSystem.h"
#include "Systems/TileSystem.h"
#include "Systems/ScoreSystem.h"
#include "AgentStates/Helping.h"
#include "AgentStates/SearchingHiddenDoor.h"
#include "AgentStates/Exploring.h"
#include "AgentStates/Seeking.h"
#include "PathFinding/PathFinder.h"

using namespace std;

void SearchingHiddenDoors::SetOrder(Agent& agent)
{
	const auto position = agent.GetPosition();
	auto& objectSystem = Locator::Get<ObjectSystem>();
	auto objectsAtPositon = objectSystem.GetInteractableObjectsAt(position);

	if (agent.HasNoOrders())
	{
		bool allWallsTested = true;
		for (const auto& wall : objectsAtPositon)
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
			agent.SetPath(AgentSystem::ConvertPathToOrder(agent, bestSearchingPath));
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

					const SOrder order = {
						.orderType = (Interact),
						.npcUID = agent.GetId(),
						.direction = object.direction,
						.objectUID = object.id,
						.interactionType = (SearchHiddenDoor)
					};
					agent.AddOrder(std::move(order));
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