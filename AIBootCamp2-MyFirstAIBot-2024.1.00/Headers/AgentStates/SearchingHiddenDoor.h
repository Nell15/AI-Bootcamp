#ifndef SEARCHING_HIDDEN_DOORS_H
#define SEARCHING_HIDDEN_DOORS_H

#include "AgentState.h"

/**
* In this state, an agent searches for hidden doors. This involves knocking on walls.
* From this state, an agent can only return to the exploration state and only once at least one tile has an exploration score greater than 0.
* An agent can only enter this state from the Exploring state, and only if every reachable tile has a score of 0.
*/
class SearchingHiddenDoors : public AgentState
{
public:
	void UpdateState(Agent& agent) override;
	void SetOrder(Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "SearchingHiddenDoors";
	}
};

#endif