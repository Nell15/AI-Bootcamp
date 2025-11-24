#ifndef HELPING_H
#define HELPING_H

#include "AgentState.h"

/**
* This state is one agents take if they have stepped on a pressure plate.
* An agent will remain in this state if other agents do not have a path to a goal or a path to a goal that passes through the door opened by the pressure plate this agent is on.
* Otherwise, an agent will leave the state and enter the Exploring state if it has no orders in it's vector of orders, or enter the Seeking state if it does.
* An agent can enter this state from the Exploring or Seeking state if it steps on a pressure plate.
*/
class Helping : public AgentState
{
public:
	void UpdateState(Agent& agent) override;
	void SetOrder(Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Helping";
	}
};

#endif