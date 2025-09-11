#ifndef UTILS_H
#define UTILS_H

#include <fstream>

#include "Algorithm/Core/Coordinates.h"

namespace Utils
{
	[[maybe_unused]] [[nodiscard]] static std::runtime_error LogAndThrow(const Coordinates& start, const Coordinates& goal)
	{
		std::ofstream log("Errors.log", std::ios::app);
		const auto message = std::format("Could not find path for start {} and goal {}\n", start, goal);

		log << message;
		return std::runtime_error(message);
	}

	[[nodiscard]] static std::runtime_error LogAndThrow(const std::string& errorMessage)
	{
		std::ofstream log("Errors.log", std::ios::app);

		log << errorMessage;
		return std::runtime_error(errorMessage);
	}
}

#endif
