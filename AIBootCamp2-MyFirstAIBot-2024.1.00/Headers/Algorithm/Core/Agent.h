#ifndef AGENT_H
#define AGENT_H

#include <vector>

#include "AgentState.h"
#include "Framework/Globals.h"

class Agent
{
public:
	Agent() = delete;
	explicit Agent(const int id) : id{ id } {}

	[[nodiscard]] int GetId() const noexcept { return id; }
	void SetPath(std::vector<EHexCellDirection> newPath) { path = std::move(newPath); }
	void AddMovement(EHexCellDirection movement) { path.emplace_back(movement); }
	void UpdateState(const LevelData& levelData) { return state->Update(levelData, *this); }

private:
	int id;
	std::vector<EHexCellDirection> path{};
	std::unique_ptr<AgentState> state{};
};

#endif