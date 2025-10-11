#ifndef COORDINATES_H
#define COORDINATES_H

#include <array>
#include <format>
#include <string>
#include <vector>

#include "Object.h"
#include "Framework/Globals.h"

struct Coordinates
{
	using DistanceType = int;

	static constexpr std::size_t NB_COORDINATES = 6;

	int q;
	int r;

	[[nodiscard]] DistanceType GetDistance(const Coordinates& tileCoord) const;
	[[nodiscard]] EHexCellDirection GetNeighborDirection(const Coordinates& neighborCoord) const;

	static bool AnyBlockingObjectInDirection(EHexCellDirection direction,
	                                         const std::vector<Object>& objectsOnTile);

	bool operator==(const Coordinates& other) const noexcept
	{
		return q == other.q && r == other.r;
	}

	Coordinates operator+(const Coordinates& other) const noexcept
	{
		return {.q = q + other.q, .r = r + other.r};
	}

	Coordinates operator-(const Coordinates& other) const noexcept
	{
		return {.q = q - other.q, .r = r - other.r};
	}

	Coordinates operator*(const int scalar) const noexcept
	{
		return {.q = q * scalar, .r = r * scalar};
	}

	static EHexCellDirection GetOppositeDirection(const Coordinates& coordinates);
	static bool IsObstacle(const Object& object);

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
		case E:      return { 0, 1 };
		case NE:     return { -1, 1 };
		case NW:     return { -1, 0 };
		case W:      return { 0, -1 };
		case SW:     return { 1, -1 };
		case SE:     return { 1, 0 };
		case CENTER: return { 0, 0 };
		default:
			throw std::out_of_range("Invalid EHexCellDirection for DirToCoordinates");
		}
	}

	static constexpr std::array<Coordinates, NB_COORDINATES> CoordinateDirections() noexcept
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
};

template <>
struct std::hash<Coordinates>
{
	std::size_t operator()(const Coordinates& coordinates) const noexcept
	{
		const std::size_t h1 = std::hash<int>{}(coordinates.q);
		const std::size_t h2 = std::hash<int>{}(coordinates.r);

		return h1 ^ h2 + 0x9e3779b97f4a7c15ULL + (h1 << 6) + (h1 >> 2);
	}
};

template <>
struct std::formatter<Coordinates> : std::formatter<std::string>
{
	auto format(const Coordinates& c, auto& ctx) const
	{
		return std::formatter<std::string>::format(
			std::format("({}, {})", c.q, c.r), ctx);
	}
};

#endif
