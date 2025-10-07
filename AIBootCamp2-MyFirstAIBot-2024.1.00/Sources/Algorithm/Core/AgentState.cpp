#include <utility>

#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"
#include "Algorithm/PathFinding/PathFinder.h"

using namespace std;

void Waiting::UpdateState(Agent& agent)
{
	// TODO: pour l'instant on fait rien
}

void Waiting::SetOrder(Agent& agent)
{
	agent.AddMovement(CENTER);
}

void Exploring::UpdateState(Agent& agent)
{
	if (not LevelData::Get().Get().GetAvailableGoalTiles().empty())
	{
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ?

		for (const Coordinates& goalTile : LevelData::Get().Get().GetAvailableGoalTiles())
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

	vector<EHexCellDirection> npcPath;
	npcPath.resize(bestExploringPath.size());

	Coordinates currCoord = agent.GetCoordinates();

	for (size_t i = 0; i < bestExploringPath.size(); ++i)
	{
		const Coordinates& nextCoord = bestExploringPath[bestExploringPath.size() - 1 - i];

		npcPath[bestExploringPath.size() - 1 - i] = currCoord.GetNeighborDirection(nextCoord);
		currCoord = nextCoord;
	}

	agent.SetPath(std::move(npcPath));
}

void Seeking::UpdateState(Agent& agent)
{
	const auto& goalTiles = LevelData::Get().GetGoalTiles();
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
	if (agent.IsPathEmpty())
	{
		PathFinder pathFinder{}; // TODO(opti): make pathfinder singleton ? Avoid calculating path every turn

		size_t bestDistance = INT_MAX;
		vector<Coordinates> bestPath{};
		for (const Coordinates& goalTile : LevelData::Get().GetAvailableGoalTiles())
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

		vector<EHexCellDirection> npcPath;
		npcPath.resize(bestPath.size());

		Coordinates currCoord = agent.GetCoordinates();

		for (size_t i = 0; i < bestPath.size(); ++i)
		{
			const Coordinates& nextCoord = bestPath[bestPath.size() - 1 - i];

			npcPath[bestPath.size() - 1 - i] = currCoord.GetNeighborDirection(nextCoord);
			currCoord = nextCoord;
		}

		agent.SetPath(std::move(npcPath));
	}
	else if (LevelData::Get().IsTileOccupied(agent.GetNextMove()))
		agent.AddMovement(CENTER);
}
