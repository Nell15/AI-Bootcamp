#include "Systems/ObjectSystem.h"

#include <span>

#include "Utils/CoordUtils.h"

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

	// Store objects
	for (const auto& objectInfo : objectInfos)
	{
		Object object =
		{
			.id = objectInfo.uid,
			.q = objectInfo.q,
			.r = objectInfo.r,
			.direction = objectInfo.cellPosition,
			.type = static_cast<EObjectType>(*objectInfo.types)
		};

		if (objectInfo.statesSize > 0)
			object.state = static_cast<EObjectState>(objectInfo.states[0]); // todo verif

		for (const auto connections = span{ objectInfo.connectedTo, static_cast<size_t>(objectInfo.connectedToSize) };
			const auto& connection : connections)
		{
			object.connectionsIds.emplace_back(connection);
		}

		Coordinates objectPos{ .q = object.q, .r = object.r };
		auto& objectAtPos = objects[objectPos];

		if (ranges::find(objectAtPos, object) == objectAtPos.end())
			objectAtPos.emplace_back(object);
	}
}

std::optional<Object> ObjectSystem::GetPressurePlateAt(Coordinates coord) const
{
	const auto objectIt = objects.find(coord);

	if (objectIt == objects.end()) return std::nullopt;

	std::vector<Object> objectsOnTile = objectIt->second;
	for (const auto& object : objectsOnTile)
	{
		if (object.type == PressurePlate) return object;
	}

	return std::nullopt;
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

std::optional<Object> ObjectSystem::GetObjectById(int id) const {
	for (const auto& [coords, vec] : objects)
	{
		auto it = std::find_if(vec.begin(), vec.end(),
			[id](const Object& obj) { return obj.id == id; });

		if (it != vec.end())
			return *it; // returns a COPY
	}

	return std::nullopt;
}
