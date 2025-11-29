#ifndef AGENT_STATE_H
#define AGENT_STATE_H

#include <string_view>

class Agent;

/**
* Our all-important, abstract class that represents the state an Agent is in.
* The children of this class decide how an agent moves by filling their vector of orders.
*/
class AgentState
{
public:
	virtual void UpdateState(Agent& agent) = 0;
	virtual void SetOrder(Agent& agent) = 0;
	[[nodiscard]] virtual std::string_view GetStateName() const = 0;

	virtual ~AgentState() = default;

	AgentState() = default;
	AgentState(const AgentState&) = default;
	AgentState(AgentState&&) = default;
	AgentState& operator=(const AgentState&) = default;
	AgentState& operator=(AgentState&&) = default;
};

#endif
