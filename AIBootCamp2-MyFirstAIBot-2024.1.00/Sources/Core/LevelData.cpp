#include "Core/LevelData.h"

#include <ranges>

#include "PathFinding/PathFinder.h"

using namespace std;

// Check if the tile is between the maximum number of columns and the maximum number of rows
bool LevelData::DoTileExist(const Coordinates tileCoord) const
{
	const int q = tileCoord.q;
	const int r = tileCoord.r;

	if (q < 0 || q >= rowCount) // 0 < q < rowCount
		return false;

	const int offset = q / 2;
	if (r < -offset || r >= colCount - offset) // 0 - (q / 2) < r < colCount - (q / 2)
		return false;

	return true;
}
