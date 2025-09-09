#include "Algorithm/Core/Coordinates.h"

#include <format>
#include <stdexcept>
#include <array>

using namespace std;

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

vector<Coordinates> Coordinates::GetNeighbors(const unordered_map<Coordinates, EHexCellType>& tiles) const
{
	vector<Coordinates> neighbors{};

	static constexpr auto directions = std::to_array<Coordinates>
	({
		Coordinates{.q = +1, .r = 0}, // E
		Coordinates{.q = +1, .r = -1}, // NE
		Coordinates{.q = 0, .r = -1}, // NW
		Coordinates{.q = -1, .r = 0}, // W
		Coordinates{.q = -1, .r = +1}, // SW
		Coordinates{.q = 0, .r = +1} // SE
	});

	for (const auto& direction : directions)
	{
		const Coordinates neighbor = *this + direction;

		if (const auto& tile = tiles.find(neighbor); tile != tiles.end() && tile->second != Forbidden)
			neighbors.emplace_back(neighbor);
	}

	return neighbors;
}
