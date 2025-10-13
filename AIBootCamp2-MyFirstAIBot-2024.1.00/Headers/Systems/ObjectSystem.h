#ifndef OBJECT_SYSTEM_H
#define OBJECT_SYSTEM_H

#include <unordered_map>

#include "Algorithm/Core/Coordinates.h"
#include "Algorithm/Core/Object.h"

class ObjectSystem
{
public:
	using ObjectList = std::unordered_map<Coordinates, std::vector<Object>>;

	static bool IsObstacle(const Object& object)
	{
		return object.type == Wall || object.type == Window;
	}

	static bool AnyBlockingObjectInDirection(EHexCellDirection direction, const std::vector<Object>& objectsOnTile)
	{
		const auto objectIt = std::ranges::find_if(objectsOnTile,
			[direction](const Object& obj)
			{
				return obj.direction == direction;
			});

		return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
	}

	void StoreObjects(const SObjectInfo* objectArrayInfo, const int nbObject)
	{
		const std::span objectInfos{ objectArrayInfo, static_cast<size_t>(nbObject) };

		for (const auto& objectInfo : objectInfos)
		{
			Object object =
			{
				.q = objectInfo.q, .r = objectInfo.r, .direction = objectInfo.cellPosition,
				.type = static_cast<EObjectType>(*objectInfo.types)
			};

			Coordinates objectCoordinates{ .q = object.q, .r = object.r };
			auto& vec = objects[objectCoordinates];

			if (std::ranges::find(vec, object) == vec.end())
				vec.emplace_back(object);
		}
	}

	[[nodiscard]] bool IsObjectBlocking(const Coordinates coordinates, const EHexCellDirection direction) const
	{
		const auto objectIt = objects.find(coordinates);

		return objectIt != objects.end() && AnyBlockingObjectInDirection(direction, objectIt->second);
	}

	static bool HasBlockingObject(const Coordinates& tileCoord, const Coordinates& directionCoord)
	{
		const Coordinates neighborPos = tileCoord + directionCoord;
		const EHexCellDirection direction = CoordUtils::CoordinatesToDir(directionCoord);
		const EHexCellDirection oppositeDirection = CoordUtils::GetOppositeDirection(directionCoord);

		const auto& objectSystem = Locator::Get<ObjectSystem>();

		if (objectSystem.IsObjectBlocking(tileCoord, direction))
			return true;

		return objectSystem.IsObjectBlocking(neighborPos, oppositeDirection);
	}

private:
	ObjectList objects{};
};

#endif