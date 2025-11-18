#pragma once

struct SNPCInfo;
struct SObjectInfo;
struct STileInfo;

/**
* A structure representing data obtained at the start of a turn.
* One of the classes given by the teacher. It has not been modified since.
*/
struct STurnData
{
	int turnNb;

	SNPCInfo* npcInfoArray;
	int npcInfoArraySize;

	STileInfo* tileInfoArray;
	int tileInfoArraySize;

	SObjectInfo* objectInfoArray;
	int objectInfoArraySize;

	int turnDelay; //in ms
};
