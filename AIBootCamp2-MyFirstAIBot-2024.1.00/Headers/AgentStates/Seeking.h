#ifndef SEEKING_H
#define SEEKING_H

#include "AgentState.h"

/**
* The state an agent might take if it thinks it can reach a goal.
* From this state, agents can enter the Helping if they step on a pressure plate, or Exploring if not path to the goal exists after all or Waiting if the agent has reached a goal.
* Only the Helping and Exploring state may turn into the Seeking state.
*/
class Seeking : public AgentState
{
public:
	void UpdateState(Agent& agent) override;
	void SetOrder(Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Seeking";
	}
};

#endif