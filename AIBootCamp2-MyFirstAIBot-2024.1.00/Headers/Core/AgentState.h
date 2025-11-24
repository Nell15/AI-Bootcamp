#ifndef AGENT_STATE_H
#define AGENT_STATE_H

#include <string_view>

class Agent;
class LevelData;

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
	void SetOrder( Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Waiting";
	}
};

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
