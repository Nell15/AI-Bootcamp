#ifndef COORDINATES_H
#define COORDINATES_H

#include <format>
#include <string>
#include <vector>

struct Coordinates
{
	int q;
	int r;

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
