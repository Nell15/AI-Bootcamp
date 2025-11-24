#ifndef LOCATOR_H
#define LOCATOR_H

#include <memory>
#include <unordered_map>
#include <typeindex>
#include <stdexcept>

/**
* The class used to get systems.
*/
class Locator
{
public:
	template <typename T>
	static void Set(const std::shared_ptr<T>& system)
	{
		systems[std::type_index(typeid(T))] = system;
	}

	template <typename T>
	static T& Get()
	{
		const auto it = systems.find(std::type_index(typeid(T)));
		if (it == systems.end())
			throw std::runtime_error("System not found");

		return *std::static_pointer_cast<T>(it->second);
	}

private:
	static inline std::unordered_map<std::type_index, std::shared_ptr<void>> systems;
};


#endif
