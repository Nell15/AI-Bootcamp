#ifndef AGENT_STATE_H
#define AGENT_STATE_H

#include "LevelData.h"

class Agent;

class AgentState
{
public:
	virtual void UpdateState(const LevelData& levelData, Agent& agent) = 0;
	virtual void SetOrder(const LevelData& levelData, Agent& agent) = 0;
	virtual std::string_view GetStateName() const = 0;

	virtual ~AgentState() = default;

	AgentState() = default;
	AgentState(const AgentState&) = default;
	AgentState(AgentState&&) = default;
	AgentState& operator=(const AgentState&) = default;
	AgentState& operator=(AgentState&&) = default;
};

class Waiting : public AgentState
{
public:
	void UpdateState(const LevelData& levelData, Agent& agent) override;
	void SetOrder(const LevelData& levelData, Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Waiting";
	}
};

class Exploring : public AgentState
{
public:
	void UpdateState(const LevelData& levelData, Agent& agent) override;
	void SetOrder(const LevelData& levelData, Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Exploring";
	}
};

class Seeking : public AgentState
{
public:
	void UpdateState(const LevelData& levelData, Agent& agent) override;
	void SetOrder(const LevelData& levelData, Agent& agent) override;

	[[nodiscard]] std::string_view GetStateName() const override
	{
		return "Seeking";
	}
};

#endif
