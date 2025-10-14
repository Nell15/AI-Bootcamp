#ifndef OBJECT_H
#define OBJECT_H

#include <optional>

#include "Framework/Globals.h"

struct Object
{
	int q;
	int r;

	EHexCellDirection direction;
	EObjectType type;

	std::optional<EObjectState> state;

	std::vector<std::pair<int, int> > connectionsIds;

	// TODO : add connections

	bool operator==(const Object& other) const noexcept
	{
		return q == other.q && r == other.r &&
			direction == other.direction && type == other.type;
	}
};

#endif
