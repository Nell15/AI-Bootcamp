#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "AgentState.h"
#include "Framework/Globals.h"

class Agent
{
public:
	Agent() = delete;

	explicit Agent(const int id, const Coordinates coord) : id{id}, coordinates{coord}, state{std::make_unique<Seeking>()}
	{
	}

	[[nodiscard]] int GetId() const noexcept { return id; }
	[[nodiscard]] Coordinates GetCoordinates() const { return coordinates; }

	void SetPath(std::vector<EHexCellDirection> newPath) { path = std::move(newPath); }
	void AddMovement(EHexCellDirection movement) { path.emplace_back(movement); }

	void UpdateState(const LevelData& levelData, const Coordinates& newCoord)
	{
		coordinates = newCoord;
		return state->Update(levelData, *this);
	}

private:
	int id;
	Coordinates coordinates;
	std::vector<EHexCellDirection> path{};
	std::unique_ptr<AgentState> state{};
};

#endif
