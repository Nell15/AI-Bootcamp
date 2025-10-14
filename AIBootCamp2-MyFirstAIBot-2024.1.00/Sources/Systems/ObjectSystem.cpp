#include "Systems/ObjectSystem.h"

#include <span>

#include "Algorithm/Utils/CoordUtils.h"

using namespace std;

bool ObjectSystem::HasBlockingObjectInDirection(EHexCellDirection direction, const vector<Object>& objectsOnTile)
{
	const auto objectIt = ranges::find_if(objectsOnTile,
		[direction](const Object& object)
		{
			return object.direction == direction;
		});

	return objectIt != objectsOnTile.end() && IsObstacle(*objectIt);
}

void ObjectSystem::StoreObjects(const SObjectInfo* objectArrayInfo, const int nbObject)
{
	const span objectInfos{ objectArrayInfo, static_cast<size_t>(nbObject) };

	for (const auto& objectInfo : objectInfos)
	{
		Object object =
		{
			.q = objectInfo.q,
			.r = objectInfo.r,
			.direction = objectInfo.cellPosition,
			.type = static_cast<EObjectType>(*objectInfo.types)
		};

		Coordinates objectPos{ .q = object.q, .r = object.r };
		auto& objectAtPos = objects[objectPos];

		if (ranges::find(objectAtPos, object) == objectAtPos.end())
			objectAtPos.emplace_back(object);
	}
}

bool ObjectSystem::IsDirectionBlocked(const Coordinates tilePos, const EHexCellDirection direction) const
{
	const auto objectIt = objects.find(tilePos);

	return objectIt != objects.end() && HasBlockingObjectInDirection(direction, objectIt->second);
}

bool ObjectSystem::IsPathBlocked(const Coordinates tilePos, const EHexCellDirection direction) const
{
	const Coordinates neighborPos = tilePos + direction;
	const EHexCellDirection oppositeDirection = CoordUtils::GetOppositeDirection(direction);

	if (IsDirectionBlocked(tilePos, direction))
		return true;

	return IsDirectionBlocked(neighborPos, oppositeDirection);
}
