#include "Systems/AgentSystem.h"

#include <ranges>

using namespace std;

Agent& AgentSystem::GetAgent(const int id)
{
	const auto agentIt = agents.find(id);

	vassert(agentIt != agents.end(), "Cannot find agent");

	return agentIt->second;
}

bool AgentSystem::IsGoalChosen(Coordinates goalPos) const
{
	return ranges::any_of(agents, [&](const auto& agentPair)
	{
		return agentPair.second.GetChosenGoal() == goalPos;
	});
}

[[nodiscard]] std::optional<std::reference_wrapper<const Agent>> AgentSystem::GetAgentAt(const Coordinates position) const
{
	for (const auto& agent : agents | views::values)
		if (agent.GetPosition() == position)
			return agent;

	return std::nullopt;
}