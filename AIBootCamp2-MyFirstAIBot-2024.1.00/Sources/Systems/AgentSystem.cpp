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

std::vector<SOrder> AgentSystem::ConvertPathToOrder(const Agent& agent, const std::vector<Coordinates>& path)
{
	vector<SOrder> npcOrders;

	npcOrders.resize(path.size());

	Coordinates currCoord = agent.GetPosition();

	for (size_t i = 0; i < path.size(); ++i)
	{
		const Coordinates& nextCoord = path[path.size() - 1 - i];
		const SOrder order =
		{
			.orderType = Move,
			.npcUID = agent.GetId(),
			.direction = CoordUtils::GetNeighborDirection(currCoord, nextCoord)
		};

		npcOrders[path.size() - 1 - i] = order;
		currCoord = nextCoord;
	}

	return npcOrders;
}
