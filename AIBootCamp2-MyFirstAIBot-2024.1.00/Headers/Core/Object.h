#ifndef OBJECT_H
#define OBJECT_H

#include <optional>

#include "Framework/Globals.h"

/**
* This structure represents objects in the level.
  Although this class represents all objects, some members are only used by specific objects, namely state for doors and connexions for pressure plates.
*/
struct Object
{
	int id;
	int q;
	int r;

	EHexCellDirection direction;
	EObjectType type;

	std::optional<EObjectState> state;

	std::vector<int> connectionsIds;

	// TODO : add connections

	bool operator==(const Object& other) const noexcept
	{
		return q == other.q
			&& r == other.r
			&& direction == other.direction;
			//&& type == other.type;
	}
};

#endif
