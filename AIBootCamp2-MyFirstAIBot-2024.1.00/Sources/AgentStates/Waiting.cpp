#include "AgentStates/Waiting.h"

#include "Core/Agent.h"

using namespace std;

void Waiting::UpdateState(Agent& agent)
{
	// Wait for ever
}

void Waiting::SetOrder(Agent& agent)
{
	// Wait for ever
	agent.AddOrder
	({
		.orderType = Move,
		.npcUID = agent.GetId(),
		.direction = CENTER
	});
}