#include "Systems/ObjectSystem.h"

#include <span>

#include "Utils/CoordUtils.h"
#include "Systems/Locator.h"
#include "Systems/TileSystem.h"

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
			object.state = static_cast<EObjectState>(objectInfo.states[0]); // todo verif case many states ?

		for (const auto connections = span{ objectInfo.connectedTo, static_cast<size_t>(objectInfo.connectedToSize) };
			const auto& connection : connections)
		{
			object.connectionsIds.emplace_back(connection);
		}

		Coordinates objectPos{ .q = object.q, .r = object.r };
		auto& objectAtPos = objects[objectPos];

		// Check if the 
		auto it = std::ranges::find(objectAtPos, object);
		if (it == objectAtPos.end())
		{
			objectAtPos.emplace_back(object);
		}
		else
		{
			*it = object;
		}
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

// todo opti : utiliser des refs partout pour eviter les copies

std::vector<Object> ObjectSystem::GetObjectsAt(Coordinates coord) const {
	auto it = objects.find(coord);
	if (it != objects.end()) {
		return it->second;
	}
	return {};
}

std::vector<Object> ObjectSystem::GetInteractableObjectsAt(Coordinates coord) const {
	std::vector<Object> interactables{};

	// Objects on the current tile
	auto it = objects.find(coord);
	if (it != objects.end()) {
		interactables.insert(interactables.end(), it->second.begin(), it->second.end());
	}
	// Object on neighbours on the side next to our tile
	auto& tileSystem = Locator::Get<TileSystem>();
	auto neightbours = tileSystem.GetNeighbors(coord);
	for (auto& neightbour : neightbours)
	{
		// get direction from neighbour to current
		auto searchedDirection = CoordUtils::GetNeighborDirection(neightbour, coord);
		// find potential object in direction
		auto neighbourObjects = this->GetObjectsAt(neightbour);
		for (auto& object : neighbourObjects)
		{
			if (IsObjectAlreadyUsed(object)) continue;
			if (object.direction == searchedDirection && object.connectionsIds.size() == 0)
				interactables.push_back(object);
		}
	}
	return interactables;
}

bool ObjectSystem::IsObjectAlreadyUsed(Object object) const {
	return std::find(usedObjects.begin(), usedObjects.end(), object) != usedObjects.end();
}

bool ObjectSystem::WallWasAlreadyTested(Object object) const {
	return std::find(searchedWalls.begin(), searchedWalls.end(), object) != searchedWalls.end();
}

size_t ObjectSystem::GetNbClosedDoorOn(const Coordinates position) const
{
	size_t nbClosedDoorOnTile = 0;
	for (auto& object : GetInteractableObjectsAt(position))
		nbClosedDoorOnTile += static_cast<size_t>(object.type == Door && object.state.has_value() && object.state == Closed);

	return nbClosedDoorOnTile;
}

void ObjectSystem::MarkUsed(Object& object) {
	if (object.type == Wall)
	{
		searchedWalls.emplace_back(object);
	}
	else
	{
		usedObjects.emplace_back(object);
	}
}
