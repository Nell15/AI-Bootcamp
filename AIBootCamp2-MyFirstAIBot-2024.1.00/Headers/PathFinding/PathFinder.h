#ifndef PATH_FINDING_H
#define PATH_FINDING_H

#include <queue>
#include <vector>
#include <optional>
#include <unordered_map>

#include "Core/Coordinates.h"

class PathFinder
{
public:
	[[nodiscard]] std::optional<std::vector<Coordinates>> FindPath(const Coordinates& start, const Coordinates& goal);

private:
	using ScoreType = int;
	using ScoreMap = std::unordered_map<Coordinates, ScoreType>;

	static constexpr ScoreType MOVEMENT_COST = 1;

	struct PQNode
	{
		Coordinates position;
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

	void Init(const Coordinates& start, const Coordinates& goal);
	void Dispose();
	void TryUpdatePath(const Coordinates& neighborPos, const PQNode& current, const Coordinates& goal);
	[[nodiscard]] static bool isGoal(const Coordinates& a, const Coordinates& b)
	{
		return a == b;
	}

	[[nodiscard]] std::vector<Coordinates> ReconstructPath(const Coordinates& start);

	std::priority_queue<PQNode, std::vector<PQNode>, PQCompare> openSet;
	std::unordered_map<Coordinates, Coordinates> predecessors;
	ScoreMap gScores;
	ScoreMap fScores;
};

#endif
