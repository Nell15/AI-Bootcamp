#ifndef UTILS_H
#define UTILS_H

#include <format>
#include <string_view>

#include "Framework/Globals.h"

namespace Utils
{
	static constexpr std::string_view to_string(const EHexCellDirection direction) noexcept
	{
		switch (direction)
		{
		case W: return "W";
		case NW: return "NW";
		case NE: return "NE";
		case E: return "E";
		case SE: return "SE";
		case SW: return "SW";
		case CENTER: return "CENTER";
		}

		return "?";
	}
}

template <>
struct std::formatter<EHexCellDirection> : std::formatter<std::string_view>
{
	auto format(const EHexCellDirection direction, std::format_context& context) const
	{
		return std::formatter<std::string_view>::format(Utils::to_string(direction), context);
	}
};

#endif
