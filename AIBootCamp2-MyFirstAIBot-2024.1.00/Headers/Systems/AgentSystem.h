#ifndef AGENT_SYSTEM_H
#define AGENT_SYSTEM_H

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "Algorithm/Core/Agent.h"

class AgentSystem
{
public:
	void AddOccupiedTiles(const Coordinates position) { occupiedPos.emplace(position); }

	void UpdateOccupiedPosition(const Coordinates oldPos, const Coordinates newPos)
	{
		occupiedPos.erase(oldPos);
		occupiedPos.emplace(newPos);
	}

	[[nodiscard]] bool IsTileOccupied(const Coordinates position) const
	{
		return occupiedPos.contains(position);
	}

	void AddAgent(Agent&& agent)
	{
		agents.emplace(agent.GetId(), std::move(agent));
	}

	Agent& GetAgent(int id);

	[[nodiscard]] bool IsGoalChosen(Coordinates goalPos) const;

private:
	std::unordered_map<int, Agent> agents{};
	std::unordered_set<Coordinates> occupiedPos{};
};

#endif
