#ifndef AGENT_SYSTEM_H
#define AGENT_SYSTEM_H

#include <algorithm>
#include <unordered_map>
#include <unordered_set>

#include "Core/Agent.h"

/**
* The system managing Agents.
  It's main role is to keep track of their position and chosen goals to help resolve conflicts.
*/
class AgentSystem
{
public:
	using AgentList = std::unordered_map<int, Agent>;

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

	[[nodiscard]] Agent& GetAgent(int id);
	[[nodiscard]] const AgentList& GetAgents() const { return agents; }

	[[nodiscard]] bool IsGoalChosen(Coordinates goalPos) const;

	[[nodiscard]] std::optional<std::reference_wrapper<const Agent>> GetAgentAt(Coordinates position) const;

	[[nodiscard]] static std::vector<SOrder> ConvertPathToOrder(const Agent& agent, const std::vector<Coordinates>& path);

private:
	AgentList agents{};
	std::unordered_set<Coordinates> occupiedPos{};
};

#endif
