#pragma once

#include <Windows.h>

/**
* A file related to the command line instructions used to run AIBootcamp. This file has not changed since it's original version given by the teacher.
*/
struct SConfigData
{
	const char* const* cmdlineStrArray;
	int cmdlineStrArraySize;
	int configDelay; //in ms
	LPSTR logpath;
};
