#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include <optional>

#include "AgentState.h"
#include "Coordinates.h"
#include "Algorithm/Utils/CoordUtils.h"
#include "Framework/Globals.h"

#include "Algorithm/Utils/VerboseAssertion.h"

class Agent
{
public:
	Agent() = delete;

	explicit Agent(const int id, const Coordinates coord) : id{id}, coordinates{coord},
	                                                        state{std::make_unique<Exploring>()}
	{
	}

	[[nodiscard]] int GetId() const { return id; }
	[[nodiscard]] Coordinates GetCoordinates() const { return coordinates; }
	[[nodiscard]] const std::optional<Coordinates>& GetChosenGoal() const { return chosenGoal; }
	[[nodiscard]] bool IsPathEmpty() const { return path.empty(); }

	[[nodiscard]] Coordinates GetNextMove() const
	{
		const auto nextMove = coordinates + CoordUtils::DirToCoordinates(path.back());
		return nextMove;
	}

	[[nodiscard]] std::string_view GetStateName() const { return state->GetStateName(); }

	void UpdateState(const Coordinates& newCoord)
	{
		coordinates = newCoord;
		state->UpdateState(*this);
	}

	void SetOrder() { state->SetOrder(*this); }
	void SetState(std::unique_ptr<AgentState> newState) { state = std::move(newState); }
	void SetChosenGoal(const std::optional<Coordinates> goal) { chosenGoal = goal; }
	void SetPath(std::vector<EHexCellDirection> newPath) { path = std::move(newPath); }

	void AddMovement(EHexCellDirection movement) { path.emplace_back(movement); }

	[[nodiscard]] EHexCellDirection PopAndReturnNextAgentMove()
	{
		vassert(!path.empty() && "Agent path is empty: cannot pop from it");

		const EHexCellDirection direction = path.back();
		path.pop_back();
		return direction;
	}

private:
	int id;
	Coordinates coordinates;
	std::optional<Coordinates> chosenGoal = std::nullopt;
	std::vector<EHexCellDirection> path{};
	std::unique_ptr<AgentState> state{};
};

#endif
