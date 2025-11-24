#ifndef EXPLORING_H
#define EXPLORING_H

#include "AgentState.h"

/**
* The starting state for all agents, representing them exploring in search of a goal.
* From this state, agents can enter Helping, Seeking or SearchHiddenDoors state.
* All of these states can also go back to exploring.
*/
class Exploring : public AgentState
{
public:
	void UpdateState(Agent& agent) override;
	void SetOrder(Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Exploring";
	}
};

#endif