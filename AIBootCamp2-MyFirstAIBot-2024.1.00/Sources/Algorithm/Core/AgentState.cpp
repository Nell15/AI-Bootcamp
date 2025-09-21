#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"

void Waiting::Update(const LevelData& levelData, Agent& agent)
{
	const auto& tiles = levelData.GetGoalTiles();

	if (std::ranges::find(tiles, agent.GetCoordinates()) != tiles.end())
		agent.AddMovement(CENTER);

	// TODO: stop waiting if the agent is not on a goal tile
	throw std::runtime_error("Waiting uodate error");
}

void Exploring::Update(const LevelData& levelData, Agent& agent)
{
	// TODO
}

void Seeking::Update(const LevelData& levelData, Agent& agent)
{
	// TODO
}
