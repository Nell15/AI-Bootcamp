#ifndef WAITING_H
#define WAITING_H

#include "AgentState.h"

/**
* This state is one agents take once they have reached a goal.
* In this state, they do nothing but wait on the goal for a game to finish.
* At the time of writing, agents cannot leave this state.
* Agents may only enter this state from the seeking state.
*/
class Waiting : public AgentState
{
public:
	void UpdateState(Agent& agent) override;
	void SetOrder(Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Waiting";
	}
};

#endif