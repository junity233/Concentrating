#include "ProcessHelper.h"
#include <Windows.h>
#include <TlHelp32.h>
#include <qstring.h>

int ProcessHelper::createProcess(const char* execPath, const char* cmdLine)
{
	STARTUPINFOA si = { 0 };
	PROCESS_INFORMATION pi = { 0 };
	DWORD pid;

	bool res = CreateProcessA(execPath, (LPSTR)cmdLine, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
	if (res)
		pid = pi.dwProcessId;
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);

	if (res)
		return pid;
	return -1;
}

bool ProcessHelper::killProcess(int pid)
{
	auto hProcess = OpenProcess(PROCESS_TERMINATE, true, pid);
	if (hProcess == NULL)
		return false;

	return TerminateProcess(hProcess, 0);
}

QList<int> ProcessHelper::findProcess(const char* name)
{
	QList<int> res;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE) {
		return QList<int>();
	}

	BOOL bResult = Process32First(hProcessSnap, &pe32);

	int num = 0;

	while (bResult)
	{
		QString processName = QString::fromWCharArray(pe32.szExeFile);
		int id = pe32.th32ProcessID;

		if (processName == name)
			res.append(id);
		bResult = Process32Next(hProcessSnap, &pe32);
	}

	CloseHandle(hProcessSnap);

	return res;
}

int ProcessHelper::currentPid()
{
	return GetCurrentProcessId();
}
