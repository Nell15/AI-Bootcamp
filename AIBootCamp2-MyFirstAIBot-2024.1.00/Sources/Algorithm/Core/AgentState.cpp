#include <utility>

#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"
#include "Algorithm/PathFinding/PathFinder.h"

using namespace std;

void Waiting::UpdateState(const LevelData& levelData, Agent& agent)
{
	// TODO: pour l'instant on fait rien
}

void Waiting::SetOrder(const LevelData& levelData, Agent& agent)
{
	agent.AddMovement(CENTER);
}

void Exploring::UpdateState(const LevelData& levelData, Agent& agent)
{
	if (not levelData.GetAvailableGoalTiles().empty())
	{
		PathFinder pathFinder{ levelData }; // TODO(opti): make pathfinder singleton ?

		for (const Coordinates& goalTile : levelData.GetAvailableGoalTiles())
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
	auto goalTiles = levelData.GetAvailableGoalTiles();
	if (goalTiles.empty())
	{
		agent.SetState(make_unique<Exploring>());
	}
	else if (ranges::find(goalTiles, agent.GetCoordinates()) != goalTiles.end())
	{
		agent.SetState(make_unique<Waiting>());
	}
}

void Seeking::SetOrder(const LevelData& levelData, Agent& agent)
{
	PathFinder pathFinder{ levelData }; // TODO(opti): make pathfinder singleton ? Avoid calculating path every turn

	size_t bestDistance = INT_MAX;
	vector<Coordinates> bestPath{};
	for (const Coordinates& goalTile : levelData.GetAvailableGoalTiles())
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
