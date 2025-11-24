#pragma once
#include <fstream>

/**
* Class used to log information into a text file.
* One of the classes given by the teacher. It has not been modified since.
*/
class Logger
{
public:
	Logger();
	virtual ~Logger();

	void Init(const std::string& _logPath, const std::string& _logFileName);
	void Log(const std::string& _data, bool _bAutoNewLine = true);

protected:

	std::ofstream mDebugFile;
};