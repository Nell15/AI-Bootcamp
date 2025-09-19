#ifndef AGENT_STATE_H
#define AGENT_STATE_H

#include "LevelData.h"

class Agent;

class AgentState
{
public:
	virtual void Update(const LevelData& levelData, Agent& agent) = 0;
	virtual ~AgentState() = default;
};

class Waiting : public AgentState
{
	void Update(const LevelData& levelData, Agent& agent) override;
};

class Exploring : public AgentState
{
	void Update(const LevelData& levelData, Agent& agent) override;
};

class Seeking : public AgentState
{
	void Update(const LevelData& levelData, Agent& agent) override;
};

#endif
