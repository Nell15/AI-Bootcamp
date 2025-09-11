#include "Algorithm/Core/Coordinates.h"

#include <format>
#include <stdexcept>
#include <array>
#include <algorithm>
#include <cassert>

#include "Algorithm/Utils/Utils.h"

using namespace std;

namespace
{
	constexpr auto coordinateDirections = to_array<Coordinates>
	({
		Coordinates{.q = 0, .r = +1}, // E
		Coordinates{.q = -1, .r = +1}, // NE
		Coordinates{.q = -1, .r = 0}, // NW
		Coordinates{.q = 0, .r = -1}, // W
		Coordinates{.q = +1, .r = -1}, // SW
		Coordinates{.q = +1, .r = 0} // SE
	});

	const unordered_map<Coordinates, EHexCellDirection> coordinateToDir =
	{
		{{.q = 0, .r = +1}, E},
		{{.q = -1, .r = +1}, NE},
		{{.q = -1, .r = 0}, NW},
		{{.q = 0, .r = -1}, W},
		{{.q = +1, .r = -1}, SW},
		{{.q = +1, .r = 0}, SE},
		{{.q = 0, .r = 0}, CENTER}
	};
}

Coordinates::DistanceType Coordinates::GetDistance(const Coordinates& goal) const
{
	const int dq = q - goal.q;
	const int dr = r - goal.r;
	const int ds = q + r - (goal.q + goal.r);

	// Hex distance formula: (|dq| + |dr| + |ds|) / 2
	return (abs(dq) + abs(dr) + abs(ds)) / 2;
}

EHexCellDirection Coordinates::GetNeighborDirection(const Coordinates& goal) const
{
	const int dq = goal.q - q;
	const int dr = goal.r - r;

	if (const auto directionIt = coordinateToDir.find(Coordinates{.q = dq, .r = dr}); directionIt != coordinateToDir.end())
		return directionIt->second;

	throw Utils::LogAndThrow(format(
		"Cannot get direction: the given cell ({},{}) is not a neighbor of the cell ({},{})",
		goal.q, goal.r, q, r));
}

bool Coordinates::AnyBlockingObjectInDirection(EHexCellDirection direction, const vector<SObjectInfo>& objectsOnTile)
{
	const auto objectIt = ranges::find_if(objectsOnTile,
	                                      [direction](const SObjectInfo& obj)
	                                      {
		                                      return obj.cellPosition == direction;
	                                      });

	return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
}

// TODO: check if not found
vector<Coordinates> Coordinates::GetNeighbors(const unordered_map<Coordinates, EHexCellType>& tiles,
                                              const unordered_map<Coordinates, vector<SObjectInfo>>& objects)
const
{
	vector<Coordinates> neighbors{};

	if (const auto objectsOnTileIt = objects.find(*this); objectsOnTileIt != objects.end())
	{
		for (const auto& coordDir : coordinateDirections)
		{
			const Coordinates neighborPos = *this + coordDir;
			const auto direction = coordinateToDir.find(coordDir)->second;
			if (not AnyBlockingObjectInDirection(direction, objectsOnTileIt->second))
			{
				auto objectsOnNeighborTile = objects.find(neighborPos);
				if (objectsOnNeighborTile != objects.end())
				{
					const EHexCellDirection oppositeDirection = GetOppositeDirection(coordDir);
					if (not AnyBlockingObjectInDirection(oppositeDirection, objectsOnNeighborTile->second))
						if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second !=
							Forbidden)
							neighbors.emplace_back(neighborPos);
				}
				else if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
					neighbors.emplace_back(neighborPos);
			}
		}
	}
	else
	{
		for (const auto& coordDir : coordinateDirections)
		{
			const Coordinates neighborPos = *this + coordDir;

			auto objectsOnNeighborTile = objects.find(neighborPos);
			if (objectsOnNeighborTile != objects.end())
			{
				const EHexCellDirection oppositeDirection = GetOppositeDirection(coordDir);
				if (not AnyBlockingObjectInDirection(oppositeDirection, objectsOnNeighborTile->second))
					if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
						neighbors.emplace_back(neighborPos);
			}
			else if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
				neighbors.emplace_back(neighborPos);
		}
	}

	return neighbors;
}

EHexCellDirection Coordinates::GetOppositeDirection(const Coordinates& coordinates)
{
	const Coordinates coord = coordinates * -1;
	const auto directionIt = coordinateToDir.find(coord);

	assert(directionIt != coordinateToDir.end() && "Cannot get opposite direction");

	return directionIt->second;
}

bool Coordinates::IsObstacle(const SObjectInfo& object)
{
	return *object.types == Wall || *object.types == Window;
}
