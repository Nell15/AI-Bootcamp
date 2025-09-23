#include <utility>

#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"
#include "Algorithm/PathFinding/PathFinder.h"

using namespace std;

void Waiting::UpdateState(const LevelData& levelData, Agent& agent)
{
	// TODO: pour l'instant ont fait rien
}

void Waiting::SetOrder(const LevelData& levelData, Agent& agent)
{
	agent.AddMovement(CENTER);
}

void Exploring::UpdateState(const LevelData& levelData, Agent& agent)
{
	const auto& goalTiles = levelData.GetGoalTiles();
	if (ranges::find(goalTiles, agent.GetCoordinates()) != goalTiles.end())
	{
		agent.SetState(make_unique<Waiting>());
	}
	else if (not levelData.GetGoalTiles().empty())
	{
		PathFinder pathFinder{ levelData }; // TODO(opti): make pathfinder singleton ?

		for (const Coordinates& goalTile : levelData.GetGoalTiles())
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

void Exploring::SetOrder(const LevelData& levelData, Agent& agent)
{
	const Coordinates agentCoord = agent.GetCoordinates();
	const auto bestNeighbor = levelData.GetBestNeighbor(agentCoord);

	agent.SetPath({agentCoord.GetNeighborDirection(bestNeighbor)});
}

void Seeking::UpdateState(const LevelData& levelData, Agent& agent)
{
	const auto& goalTiles = levelData.GetGoalTiles();
	if (ranges::find(goalTiles, agent.GetCoordinates()) != goalTiles.end())
	{
		agent.SetState(make_unique<Waiting>());
	}
	// TODO: si tous les goals connus sont occupes, passer en Exploring
}

void Seeking::SetOrder(const LevelData& levelData, Agent& agent)
{
	PathFinder pathFinder{ levelData }; // TODO(opti): make pathfinder singleton ? Avoid calculating path every turn

	size_t bestDistance = INT_MAX;
	vector<Coordinates> bestPath{};
	for (const Coordinates& goalTile : levelData.GetGoalTiles())
	{
		const auto path = pathFinder.FindPath(agent.GetCoordinates(), goalTile);
		if (path.has_value())
		{
			const auto pathSize = path.value().size();
			if (cmp_less(pathSize, bestDistance))
			{
				bestDistance = pathSize;
				bestPath = path.value();
			}
		}
	}

	// TODO: make it better
	vector<EHexCellDirection> npcPath;
	npcPath.resize(bestPath.size());
	Coordinates currCoord = agent.GetCoordinates();

	for (size_t i = 0; i < bestPath.size(); ++i)
	{
		const Coordinates& nextCoord = bestPath[i];

		npcPath[i] = currCoord.GetNeighborDirection(nextCoord);

		currCoord = nextCoord;
	}

	agent.SetPath(std::move(npcPath));
}
