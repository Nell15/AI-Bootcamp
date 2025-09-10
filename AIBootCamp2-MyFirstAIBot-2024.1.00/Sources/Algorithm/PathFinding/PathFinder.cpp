#include "Algorithm/PathFinding/PathFinder.h"

#include <algorithm>
#include <queue>
#include <stdexcept>

#include "Algorithm/Utils/Utils.h"

using namespace std;

void PathFinder::Init(const Coordinates& start, const Coordinates& goal)
{
	gScores[start] = 0;
	fScores[start] = start.GetDistance(goal);
	openSet.push({start, fScores[start], gScores[start]});
}

vector<Coordinates> PathFinder::FindPath(const Coordinates& start, const Coordinates& goal)
{
	Init(start, goal);

	while (!openSet.empty())
	{
		const auto current = openSet.top();
		openSet.pop();

		if (isGoal(current.position, goal))
			return ReconstructPath(current.position);

		for (const auto& neighbor : current.position.GetNeighbors(tilesType, objects))
			TryUpdatePath(neighbor, current, goal);
	}

	throw Utils::LogAndThrow(start, goal);
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

	// clear to allow another utilisation of the algorithm (TODO: find another solution)
	openSet = std::priority_queue<PQNode, std::vector<PQNode>, PQCompare>{};
	predecessors.clear();
	gScores.clear();
	fScores.clear();

	std::ranges::reverse(path);
	return path;
}
