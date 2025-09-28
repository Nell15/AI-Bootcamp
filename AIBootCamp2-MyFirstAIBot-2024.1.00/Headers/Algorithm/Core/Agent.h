#ifndef AGENT_H
#define AGENT_H

#include <vector>
#include <cassert>
#include <optional>

#include "AgentState.h"
#include "Coordinates.h"
#include "Framework/Globals.h"

class Agent
{
public:
	Agent() = delete;

	explicit Agent(const int id, const Coordinates coord) : id{ id }, coordinates{ coord }, state{ std::make_unique<Exploring>() }
	{
	}

	[[nodiscard]] int GetId() const noexcept { return id; }
	[[nodiscard]] Coordinates GetCoordinates() const
	{
		return coordinates;
	}

	void SetChosenGoal(const std::optional<Coordinates> goal)
	{
		chosenGoal = goal;
	}

	[[nodiscard]] const std::optional<Coordinates>& GetChosenGoal() const
	{
		return chosenGoal;
	}

	void SetState(std::unique_ptr<AgentState> newState)
	{
		state = std::move(newState);
	}
	void SetPath(std::vector<EHexCellDirection> newPath) { path = std::move(newPath); }
	void AddMovement(EHexCellDirection movement) { path.emplace_back(movement); }

	[[nodiscard]] EHexCellDirection PopAndReturnBack()
	{
		assert(not path.empty() && "Cannot pop from an empty vector");

		const EHexCellDirection direction = path.back();
		path.pop_back();
		return direction;
	}

	void UpdateState(const LevelData& levelData, const Coordinates& newCoord)
	{
		coordinates = newCoord;
		state->UpdateState(levelData, *this);
	}

	void SetOrder(const LevelData& levelData)
	{
		state->SetOrder(levelData, *this);
	}

	[[nodiscard]] bool IsPathEmpty() const { return path.empty(); }
	[[nodiscard]] Coordinates GetNextMove() const
	{
		const auto nextMove = coordinates + Coordinates::DirToCoordinates(path.back());
		return nextMove;
	}

	[[nodiscard]] std::string_view GetStateName() const
	{
		return state->GetStateName();
	}

private:
	int id;
	Coordinates coordinates;
	std::optional<Coordinates> chosenGoal = std::nullopt;
	std::vector<EHexCellDirection> path{};
	std::unique_ptr<AgentState> state{};
};

#endif
