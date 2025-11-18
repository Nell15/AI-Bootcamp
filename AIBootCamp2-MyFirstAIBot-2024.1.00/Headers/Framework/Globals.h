#pragma once

enum EHexCellType
{
	Goal,
	Forbidden,
	Default
};

enum EHexCellDirection
{
	W, NW, NE, E, SE, SW, CENTER
};

enum EObjectType
{
	Wall,
	Window,
	Door,
	PressurePlate
};

enum EObjectState
{
	Opened,
	Closed
};

enum EOrderType
{
	Move,
	Interact
};

enum EInteractionType
{
	OpenDoor,
	CloseDoor,
	SearchHiddenDoor,
};

/**
* A structure representing an order given to an Agent.
* This has been unchanged since it's original state as given by the teacher.
*/
struct SOrder
{
	EOrderType orderType;
	int npcUID;
	EHexCellDirection direction;
	int objectUID;
	EInteractionType interactionType;
};

/**
* A structure representing information about a tile.
* This has been unchanged since it's original state as given by the teacher.
*/
struct STileInfo
{
	int q;
	int r;
	EHexCellType type;
};

/**
* A structure representing information about an object.
* This has been minimally changed since it's original state as given by the teacher.
* The equal (==) operator may now be used with it.
*/
struct SObjectInfo
{
	int uid;
	int q;
	int r;
	EHexCellDirection cellPosition;

	int* types;
	int typesSize;

	int* states;
	int statesSize;

	int* connectedTo;
	int connectedToSize;

	bool operator==(const SObjectInfo& other) const noexcept
	{
		return uid == other.uid && q == other.q && r == other.r &&
			cellPosition == other.cellPosition;
	}
};

/**
* A structure representing information about an NPC.
* This has been unchanged since it's original state as given by the teacher.
*/
struct SNPCInfo
{
	int uid;
	int q;
	int r;
	int visionRange;
};
