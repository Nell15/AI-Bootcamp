#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include <optional>
#include <memory>

#include "AgentStates/AgentState.h"
#include "Coordinates.h"
#include "AgentStates/Exploring.h"
#include "Utils/CoordUtils.h"
#include "Framework/Globals.h"

#include "Utils/VerboseAssertion.h"

/**
* Our class representing one of the agents who must be moved to the goal.
  Also called NPC or player.
  Apart from the expected ID, visionRange and coordinates taken from SNPCInfo, the agent stores it's AgentState, chosen goal and a vector of orders.
*/
class Agent
{
public:
	Agent() = delete;

	explicit Agent(const int id, const int visionRange, const Coordinates coord)
		: id{id},
		  visionRange{visionRange},
		  position{coord},
		  state{std::make_unique<Exploring>()}
	{
	}

	[[nodiscard]] int GetId() const { return id; }
	[[nodiscard]] int GetVisionRange() const { return visionRange; }
	[[nodiscard]] Coordinates GetPosition() const { return position; }
	[[nodiscard]] const std::optional<Coordinates>& GetChosenGoal() const { return chosenGoal; }
	[[nodiscard]] bool HasNoOrders() const { return orders.empty(); }

	[[nodiscard]] Coordinates GetNextPosition() const
	{
		const auto nextMove = position + CoordUtils::DirToCoordinates(orders.back().direction);
		return nextMove;
	}

	[[nodiscard]] std::string_view GetStateName() const { return state->GetStateName(); }

	void UpdateState(const Coordinates& newCoord)
	{
		position = newCoord;
		state->UpdateState(*this);
	}

	void SetOrder() { state->SetOrder(*this); }
	void SetState(std::unique_ptr<AgentState> newState) { state = std::move(newState); }
	void SetChosenGoal(const std::optional<Coordinates> goal) { chosenGoal = goal; }
	void SetPath(std::vector<SOrder>&& newOrders) { orders = std::move(newOrders); }

	void AddOrder(SOrder order) { orders.emplace_back(order); }

	[[nodiscard]] SOrder PopAndReturnNextAgentMove()
	{
		vassert(!orders.empty(), "Agent orders is empty: cannot pop from it");

		const SOrder order = orders.back();
		orders.pop_back();

		return order;
	}

private:
	int id;
	int visionRange;
	Coordinates position;
	std::optional<Coordinates> chosenGoal = std::nullopt;
	std::vector<SOrder> orders{};
	std::unique_ptr<AgentState> state{};
};

#endif
