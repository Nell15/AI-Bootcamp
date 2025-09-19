#include "Algorithm/Core/AgentState.h"

#include "Algorithm/Core/Agent.h"

void Waiting::Update(const LevelData& levelData, Agent& agent)
{
	agent.AddMovement(CENTER);
}

void Exploring::Update(const LevelData& levelData, Agent& agent)
{
	// TODO
}

void Seeking::Update(const LevelData& levelData, Agent& agent)
{
	// TODO
}
