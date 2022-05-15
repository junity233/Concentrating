#include "ShutdownHelper.h"
#include <Windows.h>

bool ShutdownHelper::shutdown()
{
    HANDLE hToken;
    TOKEN_PRIVILEGES tkp;
    //��ȡ���̱�־
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
        return false;
    //��ȡ�ػ���Ȩ��LUID
    LookupPrivilegeValue(NULL, SE_SHUTDOWN_NAME, &tkp.Privileges[0].Luid);
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    //��ȡ������̵Ĺػ���Ȩ
    AdjustTokenPrivileges(hToken, false, &tkp, 0, (PTOKEN_PRIVILEGES)NULL, 0);
    if (GetLastError() != ERROR_SUCCESS) return false;
    // ǿ�ƹرռ����
    if (!ExitWindowsEx(EWX_SHUTDOWN | EWX_FORCE, 0))
        return false;
    return true;
}
