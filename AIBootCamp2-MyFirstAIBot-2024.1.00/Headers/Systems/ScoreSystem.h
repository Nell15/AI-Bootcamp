#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include "Core/Coordinates.h"
#include "Framework/Globals.h"

/**
* The system used to give score to tiles.
* Actually, the scores are only used to generate paths. This is done differently between the Exploring and SearchHiddenDoors states.
* Do not confuse the scores here, based on all sorts of tile information, on the ones in Pathfinder, based only on distance.
*/
class ScoreSystem
{
public:
	[[nodiscard]] static float CalculateScore(Coordinates position, int distance);
	[[nodiscard]] static std::vector<Coordinates> GetBestExploringPath(Coordinates position);
	[[nodiscard]] static float CalculateScoreByWalls(Coordinates position, int distance);
	[[nodiscard]] static std::vector<Coordinates> GetBestSearchingPath(Coordinates position);

private:
	using TileScore = std::pair<float, Coordinates>;

	struct ScoreCompare
	{
		const Coordinates& origin;
		explicit ScoreCompare(const Coordinates& origin) : origin(origin) {}

		bool operator()(const TileScore& a, const TileScore& b) const
		{
			return a.first < b.first;
		}
	};

	static [[nodiscard]] bool IsWorthToExplore(Coordinates position);
	static [[nodiscard]] bool CanExplore(Coordinates position, EHexCellDirection direction);
};

#endif