#ifndef COORDINATES_H
#define COORDINATES_H

#include <format>
#include <string>
#include <vector>

/**
* A structure representing axial coordinates.
* Different from STileInfo in that it represents ONLY the coordinates.
*/
struct Coordinates
{
	int q;	/**< The column number. */
	int r;	/**< The row number. */

	/**
	* Two coordinates are equal if they have the same values for q and r.
	*/
	bool operator==(const Coordinates& other) const noexcept
	{
		return q == other.q && r == other.r;
	}

	/**
	* Adding two coordinates returns a new coordinate with the sum of their q and r values.
	*/
	Coordinates operator+(const Coordinates& other) const noexcept
	{
		return {.q = q + other.q, .r = r + other.r};
	}

	/**
	* Adding two coordinates returns a new coordinate with the difference of their q and r values.
	*/
	Coordinates operator-(const Coordinates& other) const noexcept
	{
		return {.q = q - other.q, .r = r - other.r};
	}

	/**
	* A coordinate may be multiplied by a scalar number which is then multiplied into it's q and r values.
	*/
	Coordinates operator*(const int scalar) const noexcept
	{
		return {.q = q * scalar, .r = r * scalar};
	}
};

/**
* This structure represents a hashed coordinate, necessary for storing sets of coordinates.
*/
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

/**
* This structure makes it easy to print a coordinate.
*/
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
