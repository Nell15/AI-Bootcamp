#ifndef COORD_UTILS_H
#define COORD_UTILS_H

#include <array>

#include "Core/Coordinates.h"
#include "Framework/Globals.h"

namespace CoordUtils
{
	using DistanceType = int;

	static constexpr std::size_t NB_DIRECTION = 7;
	static constexpr std::size_t NB_NEIGHBOR_DIRECTION = 6;

	static constexpr std::array neighborDirection =
	{
		E,
		NE,
		NW,
		W,
		SW,
		SE,
	};

	static constexpr EHexCellDirection CoordinatesToDir(const Coordinates& coord)
	{
		if (coord.q == 0 && coord.r == +1) return E;
		if (coord.q == -1 && coord.r == +1) return NE;
		if (coord.q == -1 && coord.r == 0) return NW;
		if (coord.q == 0 && coord.r == -1) return W;
		if (coord.q == +1 && coord.r == -1) return SW;
		if (coord.q == +1 && coord.r == 0) return SE;
		if (coord.q == 0 && coord.r == 0) return CENTER;

		throw std::out_of_range("Invalid Coordinates for CoordinatesToDir");
	}

	static constexpr Coordinates DirToCoordinates(const EHexCellDirection dir)
	{
		switch (dir)
		{
		case E:      return { .q = 0, .r = 1 };
		case NE:     return { .q = -1, .r = 1 };
		case NW:     return { .q = -1, .r = 0 };
		case W:      return { .q = 0, .r = -1 };
		case SW:     return { .q = 1, .r = -1 };
		case SE:     return { .q = 1, .r = 0 };
		case CENTER: return { .q = 0, .r = 0 };
		}

		throw std::out_of_range("Invalid EHexCellDirection for DirToCoordinates");
	}

	static constexpr std::array<Coordinates, NB_NEIGHBOR_DIRECTION> CoordinateDirections() noexcept
	{
		return std::to_array<Coordinates>
			({
				Coordinates{.q = 0, .r = +1}, // E
				Coordinates{.q = -1, .r = +1}, // NE
				Coordinates{.q = -1, .r = 0}, // NW
				Coordinates{.q = 0, .r = -1}, // W
				Coordinates{.q = +1, .r = -1}, // SW
				Coordinates{.q = +1, .r = 0} // SE
				});
	}

	static constexpr EHexCellDirection GetOppositeDirection(const Coordinates& coord)
	{
		return CoordinatesToDir(coord * -1);
	}

	static constexpr EHexCellDirection GetOppositeDirection(const EHexCellDirection direction)
	{
		switch (direction)
		{
		case W:  return E;
		case NW: return SE;
		case NE: return SW;
		case E:  return W;
		case SE: return NW;
		case SW: return NE;
		case CENTER: return CENTER;
		}

		throw std::out_of_range("Invalid EHexCellDirection for GetOppositeDirection");
	}

	static DistanceType GetDistance(const Coordinates& source, const Coordinates& destination)
	{
		const int dq = source.q - destination.q;
		const int dr = source.r - destination.r;
		const int ds = source.q + source.r - (destination.q + destination.r);

		// Hex distance formula: (|dq| + |dr| + |ds|) / 2
		return (abs(dq) + abs(dr) + abs(ds)) / 2;
	}

	static constexpr EHexCellDirection GetNeighborDirection(const Coordinates& source, const Coordinates& neighbor)
	{
		const int dq = neighbor.q - source.q;
		const int dr = neighbor.r - source.r;

		return CoordinatesToDir(Coordinates{ .q = dq, .r = dr });
	}
}

static Coordinates operator+(const Coordinates& coords, const EHexCellDirection dir) noexcept
{
	const auto offset = CoordUtils::DirToCoordinates(dir);
	return coords + offset;
}

static Coordinates operator+(const EHexCellDirection dir, const Coordinates& coords) noexcept
{
	return coords + dir;
}

static Coordinates& operator+=(Coordinates& coords, const EHexCellDirection dir) noexcept
{
	const auto offset = CoordUtils::DirToCoordinates(dir);
	coords = coords + offset;
	return coords;
}
#endif