#pragma once

struct SNPCInfo;
struct SObjectInfo;
struct STileInfo;

/**
* A structure representing data obtained at the start of a level.
* One of the classes given by the teacher. It has not been modified since.
*/
struct SInitData
{
	int initDelay; //in ms

	int rowCount;
	int colCount;

	int turnDelay; //in ms
	int maxTurnNb;

	bool omniscient;

	SNPCInfo* npcInfoArray;
	int nbNPCs;

	STileInfo* tileInfoArray;
	int tileInfoArraySize;

	SObjectInfo* objectInfoArray;
	int objectInfoArraySize;
};
