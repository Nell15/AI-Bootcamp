#include "Algorithm/Core/Coordinates.h"

#include <format>
#include <algorithm>

using namespace std;

auto Coordinates::GetDistance(const Coordinates& tileCoord) const -> DistanceType
{
	const int dq = q - tileCoord.q;
	const int dr = r - tileCoord.r;
	const int ds = q + r - (tileCoord.q + tileCoord.r);

	// Hex distance formula: (|dq| + |dr| + |ds|) / 2
	return (abs(dq) + abs(dr) + abs(ds)) / 2;
}

EHexCellDirection Coordinates::GetNeighborDirection(const Coordinates& neighborCoord) const
{
	const int dq = neighborCoord.q - q;
	const int dr = neighborCoord.r - r;

	return CoordinatesToDir(Coordinates{.q = dq, .r = dr});
}

bool Coordinates::AnyBlockingObjectInDirection(EHexCellDirection direction, const vector<Object>& objectsOnTile)
{
	const auto objectIt = ranges::find_if(objectsOnTile,
	                                      [direction](const Object& obj)
	                                      {
		                                      return obj.direction == direction;
	                                      });

	return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
}

EHexCellDirection Coordinates::GetOppositeDirection(const Coordinates& coordinates)
{
	return CoordinatesToDir(coordinates * -1);
}

bool Coordinates::IsObstacle(const Object& object)
{
	return object.type == Wall || object.type == Window;
}
