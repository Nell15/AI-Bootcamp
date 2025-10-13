#ifndef OBJECT_SYSTEM_H
#define OBJECT_SYSTEM_H

#include <unordered_map>

#include "Algorithm/Core/Coordinates.h"
#include "Algorithm/Core/Object.h"

class ObjectSystem
{
public:
	using ObjectList = std::unordered_map<Coordinates, std::vector<Object>>;

	[[nodiscard]] bool IsPathBlocked(Coordinates tilePos, EHexCellDirection direction) const;
	void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

private:
	ObjectList objects{};

	static bool IsObstacle(const Object& object)
	{
		return object.type == Wall || object.type == Window;
	}

	[[nodiscard]] static bool HasBlockingObjectInDirection(EHexCellDirection direction, const std::vector<Object>& objectsOnTile);
	[[nodiscard]] bool IsDirectionBlocked(Coordinates tilePos, EHexCellDirection direction) const;
	
};

#endif