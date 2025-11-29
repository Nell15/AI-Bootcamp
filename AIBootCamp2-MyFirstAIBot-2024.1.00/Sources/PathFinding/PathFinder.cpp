#include "PathFinding/PathFinder.h"

#include <queue>

#include "Core/LevelData.h"
#include "Systems/Locator.h"
#include "Systems/TileSystem.h"
#include "Utils/CoordUtils.h"

using namespace std;

void PathFinder::Init(const Coordinates& start, const Coordinates& goal)
{
	// Initialize cost for the start node
	gScores[start] = 0;
	fScores[start] = CoordUtils::GetDistance(start, goal);

	// Push start node into the open set
	openSet.push({start, fScores[start], gScores[start]});
}

void PathFinder::Dispose()
{
	// Reset all A* state
	openSet = priority_queue<PQNode, vector<PQNode>, PQCompare>{};
	predecessors.clear();
	gScores.clear();
	fScores.clear();
}

optional<vector<Coordinates>> PathFinder::FindPath(const Coordinates& start, const Coordinates& goal)
{
	const auto& tileSystem = Locator::Get<TileSystem>();
	Init(start, goal);

	while (!openSet.empty())
	{
		// Get node with the lowest F score
		const PQNode current = openSet.top();
		openSet.pop();

		// If goal reached, reconstruct and return the path
		if (current.position == goal)
		{
			auto path = ReconstructPath(current.position);
			Dispose();
			return path;
		}

		// Explore all reachable neighboring tiles
		for (const auto& neighbor : tileSystem.GetWalkableNeighbors(current.position))
			TryUpdatePath(neighbor, current, goal);
	}

	Dispose();
	return nullopt;
}

void PathFinder::TryUpdatePath(const Coordinates& neighborPos,
                               const PQNode& current,
                               const Coordinates& goal)
{
	// Cost from start to neighbor through current
	const ScoreType tentativeG = current.g + MOVEMENT_COST;

	// If the neighbor not visited or if we found a shorter path
	if (const auto neighIt = gScores.find(neighborPos);
		neighIt == gScores.end() || tentativeG < neighIt->second)
	{
		// Recalculate scores
		const ScoreType tentativeH = CoordUtils::GetDistance(neighborPos, goal);
		const ScoreType tentativeF = tentativeG + tentativeH;

		// Update path and scores
		predecessors[neighborPos] = current.position;
		gScores[neighborPos] = tentativeG;
		fScores[neighborPos] = tentativeF;

		// Push updated node into the priority queue
		openSet.push(PQNode{.position = neighborPos, .f = tentativeF, .g = tentativeG});
	}
}

vector<Coordinates> PathFinder::ReconstructPath(const Coordinates& start)
{
	vector<Coordinates> path;
	Coordinates node = start;

	path.emplace_back(node);

	auto currentIt = predecessors.find(node);
	while (currentIt != predecessors.end())
	{
		node = currentIt->second;
		path.emplace_back(node);
		currentIt = predecessors.find(node);
	}

	path.pop_back();
	return path;
}
