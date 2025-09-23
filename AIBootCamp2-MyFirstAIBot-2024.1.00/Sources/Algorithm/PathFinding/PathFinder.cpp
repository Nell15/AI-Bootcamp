#include "Algorithm/PathFinding/PathFinder.h"

#include <algorithm>
#include <queue>

using namespace std;

void PathFinder::Init(const Coordinates& start, const Coordinates& goal)
{
	gScores[start] = 0;
	fScores[start] = start.GetDistance(goal);
	openSet.push({start, fScores[start], gScores[start]});
}

void PathFinder::Dispose()
{
	openSet = priority_queue<PQNode, vector<PQNode>, PQCompare>{};
	predecessors.clear();
	gScores.clear();
	fScores.clear();
}

optional<vector<Coordinates>> PathFinder::FindPath(const Coordinates& start, const Coordinates& goal)
{
	Init(start, goal);

	while (!openSet.empty())
	{
		const PQNode current = openSet.top();
		openSet.pop();

		if (isGoal(current.position, goal))
		{
			auto path = ReconstructPath(current.position);
			Dispose();
			return path;
		}

		for (const auto& neighbor : levelData.GetWalkableNeighbors(current.position))
			TryUpdatePath(neighbor, current, goal);
	}

	Dispose();
	return nullopt;
}

void PathFinder::TryUpdatePath(const Coordinates& neighborPos,
                               const PQNode& current,
                               const Coordinates& goal)
{
	const ScoreType tentativeG = current.g + MOVEMENT_COST;

	if (const auto neighIt = gScores.find(neighborPos);
		neighIt == gScores.end() || tentativeG < neighIt->second)
	{
		const ScoreType tentativeH = neighborPos.GetDistance(goal);
		const ScoreType tentativeF = tentativeG + tentativeH;

		predecessors[neighborPos] = current.position;
		gScores[neighborPos] = tentativeG;
		fScores[neighborPos] = tentativeF;

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
