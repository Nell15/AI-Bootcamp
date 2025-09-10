#ifndef COORDINATES_H
#define COORDINATES_H

#include <format>
#include <string>
#include <unordered_map>
#include <vector>

#include "Framework/Globals.h"

struct Coordinates
{
	using DistanceType = int;

	int q;
	int r;

	[[nodiscard]] DistanceType GetDistance(const Coordinates& goal) const;
	[[nodiscard]] EHexCellDirection GetNeighborDirection(const Coordinates& goal) const;
	[[nodiscard]] std::vector<Coordinates> GetNeighbors(
		const std::unordered_map<Coordinates, EHexCellType>& tiles, const std::unordered_map<Coordinates, std::vector<SObjectInfo>>& objects) const;

	static bool AnyBlockingObjectInDirection(EHexCellDirection direction, const std::vector<SObjectInfo>& objectsOnTile);

	bool operator==(const Coordinates& other) const noexcept
	{
		return q == other.q && r == other.r;
	}

	Coordinates operator+(const Coordinates& other) const noexcept
	{
		return {.q = q + other.q, .r = r + other.r};
	}

	Coordinates operator*(const int scalar) const noexcept
	{
		return {.q = q * scalar, .r = r * scalar};
	}
private:
	static EHexCellDirection GetOppositeDirection(Coordinates direction);
	static bool IsObstacle(SObjectInfo object);
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
