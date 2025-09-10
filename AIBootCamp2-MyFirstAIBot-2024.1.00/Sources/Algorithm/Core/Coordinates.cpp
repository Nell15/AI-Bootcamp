#include "Algorithm/Core/Coordinates.h"

#include <format>
#include <stdexcept>
#include <array>
#include <algorithm>

using namespace std;

namespace
{
	constexpr auto coordinateDirections = std::to_array<Coordinates>
	({
		Coordinates{.q = 0, .r = +1}, // E
		Coordinates{.q = -1, .r = +1}, // NE
		Coordinates{.q = -1, .r = 0}, // NW
		Coordinates{.q = 0, .r = -1}, // W
		Coordinates{.q = +1, .r = -1}, // SW
		Coordinates{.q = +1, .r = 0} // SE
	});

	const unordered_map<Coordinates, EHexCellDirection> coordinatesToDir = 
	{
		{{0, +1}, E},
		{{-1, +1}, NE},
		{{-1, 0}, NW},
		{{0, -1}, W},
		{{+1, -1}, SW},
		{{+1, 0}, SE},
		{{0, 0}, CENTER}
	};
}

Coordinates::DistanceType Coordinates::GetDistance(const Coordinates& goal) const
{
	const int dq = q - goal.q;
	const int dr = r - goal.r;
	const int ds = q + r - (goal.q + goal.r);

	// Hex distance formula: (|dq| + |dr| + |ds|) / 2
	return (std::abs(dq) + std::abs(dr) + std::abs(ds)) / 2;
}

EHexCellDirection Coordinates::GetNeighborDirection(const Coordinates& goal) const
{
	const int dq = goal.q - q;
	const int dr = goal.r - r;

	if (dq == 0 && dr == 0) return CENTER;

	if (dq == 0 && dr == -1) return W;
	if (dq == -1 && dr == 0) return NW;
	if (dq == -1 && dr == +1) return NE;
	if (dq == 0 && dr == +1) return E;
	if (dq == +1 && dr == 0) return SE;
	if (dq == +1 && dr == -1) return SW;

	// TODO: log the error ?
	throw std::runtime_error(
		std::format("Cannot get direction: the given cell ({},{}) is not a neighbor of the cell ({},{})",
		            goal.q, goal.r, q, r));
}

bool Coordinates::AnyBlockingObjectInDirection(EHexCellDirection direction, const vector<SObjectInfo>& objectsOnTile)
{
	auto objectIt = ranges::find_if(objectsOnTile,
	                          [direction](SObjectInfo obj) { return obj.cellPosition == direction; });	

	return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
}

// TODO: check if not found
vector<Coordinates> Coordinates::GetNeighbors(const unordered_map<Coordinates, EHexCellType>& tiles,
	const std::unordered_map<Coordinates, std::vector<SObjectInfo>>& objects) const
{
	vector<Coordinates> neighbors{};

	if (auto objectsOnTileIt = objects.find(*this); objectsOnTileIt != objects.end())
	{
		for (const auto& coordDir : coordinateDirections)
		{
			const Coordinates neighborPos = *this + coordDir;
			const auto direction = coordinatesToDir.find(coordDir)->second;
			if (not AnyBlockingObjectInDirection(direction, objectsOnTileIt->second))
			{
				auto objectsOnNeighborTile = objects.find(neighborPos);
				if (objectsOnNeighborTile != objects.end())
				{
					const EHexCellDirection oppositeDirection = GetOppositeDirection(coordDir);
					if (not AnyBlockingObjectInDirection(oppositeDirection, objectsOnNeighborTile->second))
						if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
							neighbors.emplace_back(neighborPos);
				}
				else
					if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
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
			else
				if (const auto& tile = tiles.find(neighborPos); tile != tiles.end() && tile->second != Forbidden)
					neighbors.emplace_back(neighborPos);
		}
	}

	return neighbors;
}

EHexCellDirection Coordinates::GetOppositeDirection(Coordinates coordinates)
{
	const Coordinates coord = coordinates * -1;
	const auto direction = coordinatesToDir.find(coord);

	// TODO: handle errors

	return direction->second;
}

bool Coordinates::IsObstacle(SObjectInfo object)
{
	return *object.types == EObjectType::Wall || *object.types == EObjectType::Window;
}
