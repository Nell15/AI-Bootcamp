#ifndef UTILS_H
#define UTILS_H

#include <fstream>

namespace Utils
{
	[[nodiscard]] static std::runtime_error LogAndThrow(const Coordinates& start, const Coordinates& goal)
	{
		std::ofstream log("AAA-Errors.log", std::ios::app);
		const auto message = std::format("Could not find path for start {} and goal {}\n", start, goal);

		log << message;
		return std::runtime_error(message);
	}
}

#endif
