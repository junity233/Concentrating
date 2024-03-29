#pragma once
#include <qlist.h>

class ProcessHelper
{
public:
	static int createProcess(const char* execPath, const char* cmdLine);
	static bool killProcess(int pid);
	static QList<int> findProcess(const char* name);
	static int currentPid();
};

