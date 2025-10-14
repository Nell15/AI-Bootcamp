#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include "Core/Coordinates.h"
#include "Framework/Globals.h"

class ScoreSystem
{
public:
	[[nodiscard]] static int CalculateScore(Coordinates position);
	[[nodiscard]] static std::vector<Coordinates> GetBestExploringPath(Coordinates position);

private:
	using TileScore = std::pair<int, Coordinates>;

	struct MinScoreCompare
	{
		bool operator()(const TileScore& a, const TileScore& b) const
		{
			return a.first < b.first;
		}
	};

	static [[nodiscard]] bool IsWorthToExplore(Coordinates position);
	static [[nodiscard]] bool CanExplore(Coordinates position, EHexCellDirection direction);
};

#endif