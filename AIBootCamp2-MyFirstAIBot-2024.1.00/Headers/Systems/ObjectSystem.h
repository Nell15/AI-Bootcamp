#ifndef OBJECT_SYSTEM_H
#define OBJECT_SYSTEM_H

#include <unordered_map>

#include "Core/Coordinates.h"
#include "Core/Object.h"

class ObjectSystem
{
public:
	using ObjectList = std::unordered_map<Coordinates, std::vector<Object>>;

	[[nodiscard]] bool IsPathBlocked(Coordinates tilePos, EHexCellDirection direction) const;

	void StoreObjects(const SObjectInfo* objectArrayInfo, int nbObject);

	std::optional<Object> GetPressurePlateAt(Coordinates coord) const;

	std::optional<Object> GetObjectById(int id) const;

	std::vector<Object> GetObjectsAt(Coordinates coord) const;

	std::vector<Object> GetInteractableObjectsAt(Coordinates coord) const;

	bool IsObjectAlreadyUsed(Object object) const; // todo: might need separation for knock & open
	bool WallWasAlreadyTested(Object object) const; // todo: might need separation for knock & open

	[[nodiscard]] size_t GetNbClosedDoorOn(Coordinates position) const;

	void MarkUsed(Object& object);

	[[nodiscard]] bool HasObject() const { return not objects.empty(); }

private:
	ObjectList objects{};
	std::vector<Object> usedObjects; // todo: change this for optimisation ?
	std::vector<Object> searchedWalls; // todo: change this for optimisation ?

	static bool IsObstacle(const Object& object)
	{
		return object.type == Wall
			|| object.type == Window
			|| (object.type == Door && object.state.has_value() && object.state.value() == Closed);
	}

	[[nodiscard]] static bool HasBlockingObjectInDirection(EHexCellDirection direction, const std::vector<Object>& objectsOnTile);
	[[nodiscard]] bool IsDirectionBlocked(Coordinates tilePos, EHexCellDirection direction) const;

};

#endif