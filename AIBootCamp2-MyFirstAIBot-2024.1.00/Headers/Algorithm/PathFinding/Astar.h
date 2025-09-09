#ifndef ASTAR_H
#define ASTAR_H

#include <queue>

#include "Algorithm/Core/Coordinates.h"
#include "PathFinding.h"
#include "Framework/Globals.h"

using namespace std;

class Astar : public PathFinding
{
public:

private:
};

static vector<Coordinates> ReconstructPath(
	const unordered_map<Coordinates, Coordinates>& cameFrom,
	Coordinates current
)
{
	vector<Coordinates> totalPath;
	totalPath.push_back(current);

	auto it = cameFrom.find(current);
	while (it != cameFrom.end())
	{
		current = it->second;
		totalPath.push_back(current);
		it = cameFrom.find(current);
	}

	reverse(totalPath.begin(), totalPath.end());
	return totalPath;
}

static vector<Coordinates> Astar(Coordinates start, Coordinates goal,
	const unordered_map<Coordinates, EHexCellType>& tiles)
{
	using ScoreType = int;

	struct PQNode
	{
		Coordinates coord;
		ScoreType f;
		ScoreType g;
	};

	struct PQCompare
	{
		bool operator()(const PQNode& a, const PQNode& b) const noexcept
		{
			if (a.f != b.f) return a.f > b.f;
			return a.g < b.g;
		}
	};

	priority_queue<PQNode, vector<PQNode>, PQCompare> openPQ;
	unordered_map<Coordinates, ScoreType> gScore;
	unordered_map<Coordinates, ScoreType> fScore;
	unordered_map<Coordinates, Coordinates> cameFrom;

	gScore[start] = 0;
	fScore[start] = start.GetDistance(goal);
	openPQ.push({ start, fScore[start], gScore[start] });

	while (!openPQ.empty())
	{
		auto node = openPQ.top();
		openPQ.pop();

		if (node.coord.q == goal.q && node.coord.r == goal.r)
		{
			auto a = ReconstructPath(cameFrom, node.coord);
			return a;
		}

		const Coordinates n = { node.coord.q, node.coord.r };
		for (const auto& neigh : n.GetNeighbors(tiles))
		{
			ScoreType tentative_g = node.g + 1;

			if (auto itg = gScore.find(neigh); itg == gScore.end() || tentative_g < itg->second)
			{
				cameFrom[neigh] = node.coord;
				gScore[neigh] = tentative_g;
				ScoreType h = neigh.GetDistance(goal);
				ScoreType f = tentative_g + h;
				fScore[neigh] = f;
				openPQ.push(PQNode{ neigh, f, tentative_g });
			}
		}
	}

	return {};
}

#endif