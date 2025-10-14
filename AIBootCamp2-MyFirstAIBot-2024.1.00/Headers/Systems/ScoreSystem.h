#ifndef SCORE_SYSTEM_H
#define SCORE_SYSTEM_H

#include "Core/Coordinates.h"
#include "Framework/Globals.h"
#include "Utils/CoordUtils.h"

class ScoreSystem
{
public:
	[[nodiscard]] static float CalculateScore(Coordinates position, int distance);
	[[nodiscard]] static std::vector<Coordinates> GetBestExploringPath(Coordinates position);

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