// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include <windows.h>
#include "hook.h"
#include "detours.h"
#include <TlHelp32.h>
#include <stdio.h>
#include <string.h>
#include <process.h>

static HHOOK keyboardHook = NULL, mouseHook = NULL;

static HINSTANCE hins = NULL;

#pragma data_seg("shared")

static DWORD protectedProcessID = 0;        //进程防杀主进程的PID——该进程会被保护
static WCHAR dllFullPath[MAX_PATH] = L"";   //hook.dll的路径（其他进程无法直接获取）

#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")  //设置为共享内存


static BOOL exceptedKeys[0xff];
/**
 * 几个API Hook
*/

static HANDLE(WINAPI* orginOpenProcess)(DWORD, BOOL, DWORD) = OpenProcess;

static BOOL(WINAPI* orginCreateProcessA)(
    LPCSTR                lpApplicationName,
    LPSTR                 lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCSTR                lpCurrentDirectory,
    LPSTARTUPINFOA        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    ) = CreateProcessA;

static BOOL(WINAPI* orginCreateProcessW)(
    LPCWSTR               lpApplicationName,
    LPWSTR                lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCWSTR               lpCurrentDirectory,
    LPSTARTUPINFOW        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
    ) = CreateProcessW;


/**
 * @brief 安装进程防杀到一个进程
 * @param hProcess
 * @return
*/
static BOOL InstallProcPectHookForProcess(HANDLE hProcess);
/**
 * @brief 从一个进程中卸载进程防杀
 * @param pid
 * @return
*/
static BOOL UninstallProcPectHookForProcess(DWORD pid);
/**
 * @brief Hook进程防杀的API
 * @return
*/
static BOOL HookAPI();
/**
 * @brief Unhook进程防杀的API
 * @return
*/
static BOOL UnhookAPI();

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        hins = hModule;
        HookAPI();              //加载DLL时，hook相关API
        break;

    case DLL_PROCESS_DETACH:
        UnhookAPI();            //卸载时，也卸载API
        break;
    }

    return TRUE;
}

//键盘钩子回调
LRESULT CALLBACK KeyboardHookHandle(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode < 0) {
        //Microsoft docs规定nCode为0时必须下传消息
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    }
    //获取虚拟键码
    DWORD vkCode = ((LPKBDLLHOOKSTRUCT)lParam)->vkCode;

    //debug下排除delete键
#ifdef _DEBUG
    if (vkCode == VK_DELETE)
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
#endif

    if (exceptedKeys[vkCode]) {
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);
    }
    return 1;
}

//替换原版OpenProcess的函数
HANDLE WINAPI HookedOpenProcess(DWORD dwDesiredAccess, BOOL bInheritHandle, DWORD dwProcessId)
{
    //防杀
    if (dwProcessId != protectedProcessID)
        return (*orginOpenProcess)(dwDesiredAccess, bInheritHandle, dwProcessId);
    SetLastError(ERROR_ACCESS_DENIED);
    return NULL;
}

BOOL WINAPI HookedCreateProcessW(
    LPCWSTR               lpApplicationName,
    LPWSTR                lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCWSTR               lpCurrentDirectory,
    LPSTARTUPINFOW        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    BOOL res;

    res = orginCreateProcessW(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);
    //调用原始的CreateProcess函数，若创建进程成功，则注入dll
    if (res != 0) {
        BOOL res;
        res = InstallProcPectHookForProcess(lpProcessInformation->hProcess);

    }


    return res;
}

BOOL WINAPI HookedCreateProcessA(
    LPCSTR                lpApplicationName,
    LPSTR                 lpCommandLine,
    LPSECURITY_ATTRIBUTES lpProcessAttributes,
    LPSECURITY_ATTRIBUTES lpThreadAttributes,
    BOOL                  bInheritHandles,
    DWORD                 dwCreationFlags,
    LPVOID                lpEnvironment,
    LPCSTR                lpCurrentDirectory,
    LPSTARTUPINFOA        lpStartupInfo,
    LPPROCESS_INFORMATION lpProcessInformation
) {
    BOOL res;

    res = orginCreateProcessA(lpApplicationName, lpCommandLine, lpProcessAttributes, lpThreadAttributes, bInheritHandles,
        dwCreationFlags, lpEnvironment, lpCurrentDirectory, lpStartupInfo, lpProcessInformation);

    if (res != 0) {
        BOOL res;
        res = InstallProcPectHookForProcess(lpProcessInformation->hProcess);


    }
    return res;
}

//鼠标钩子回调
LRESULT CALLBACK MouseHookHandle(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode < 0)
        return CallNextHookEx(keyboardHook, nCode, wParam, lParam);

    return 1;
}

BOOL  InstallKeyboardHook() {
    keyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardHookHandle, hins, 0);
    if (keyboardHook != NULL) {
        return TRUE;
    }
    return FALSE;
}

BOOL UninstallKeyboardHook() {
    if (keyboardHook != NULL) {
        return UnhookWindowsHookEx(keyboardHook);
    }
    return TRUE;
}

BOOL InstallMouseHook() {
    mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookHandle, hins, 0);
    if (mouseHook != NULL) {
        return TRUE;
    }
    return FALSE;
}

BOOL UninstallMouseHook() {
    if (mouseHook != NULL) {
        return UnhookWindowsHookEx(mouseHook);
    }
    return TRUE;
}

static BOOL InstallProcPectHookForProcess(HANDLE hProcess) {

    LPVOID loadLibraryAddr = DetourFindFunction("kernel32.dll", "LoadLibraryW");
    LPVOID llPraram = VirtualAllocEx(hProcess, NULL, wcslen(dllFullPath), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (llPraram == NULL) {
        FreeLibrary(loadLibraryAddr);
        return FALSE;
    }

    //wchar为2字节，还要加上最后的\0
    WriteProcessMemory(hProcess, llPraram, dllFullPath, wcslen(dllFullPath) * 2 + 2, NULL);

    HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)loadLibraryAddr, llPraram, 0, NULL);
    if (hThread) {
        ResumeThread(hThread);

        CloseHandle(hThread);
        return TRUE;
    }
    return FALSE;
}

BOOL InstallProcPectHook()
{
    WCHAR dirPath[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, dirPath);

    wsprintfW(dllFullPath, L"%ls\\" HOOK_DLL_NAME, dirPath);

    /*遍历进程*/

    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    BOOL more = Process32First(hSnapshot, &process);

    while (more) {
        HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
            PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, process.th32ProcessID);

        if (hProcess) {
            InstallProcPectHookForProcess(hProcess);

            CloseHandle(hProcess);
        }

        more = Process32Next(hSnapshot, &process);
    }

    CloseHandle(hSnapshot);
    return TRUE;
}

static BOOL UninstallProcPectHookForProcess(DWORD pid) {

    /*遍历该进程所有dll*/
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, pid);

    if (hSnapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    MODULEENTRY32 module;
    module.dwSize = sizeof(module);

    BOOL more = Module32First(hSnapshot, &module);

    while (more) {
        if (lstrcmpW(module.szModule, HOOK_DLL_NAME) == 0) {//找到hook.dll
            HANDLE hProcess = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_VM_OPERATION |
                PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, pid);

            LPVOID freeLibrary = DetourFindFunction("kernel32.dll", "FreeLibrary");
            LPVOID llPraram = VirtualAllocEx(hProcess, NULL, sizeof(LPVOID), MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);


            HANDLE hThread = CreateRemoteThread(
                hProcess,
                NULL,
                0,
                (LPTHREAD_START_ROUTINE)freeLibrary,
                module.modBaseAddr,
                0,
                NULL
            );

            if (hThread) {
                ResumeThread(hThread);

                CloseHandle(hThread);
            }

            CloseHandle(hProcess);
            break;
        }

        more = Module32Next(hSnapshot, &module);
    }
    CloseHandle(hSnapshot);

    return TRUE;
}

BOOL UninstallProcPectHook()
{
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE)
        return FALSE;

    PROCESSENTRY32 process;
    process.dwSize = sizeof(process);

    BOOL more = Process32First(hSnapshot, &process);

    while (more) {
        UninstallProcPectHookForProcess(process.th32ProcessID);

        more = Process32Next(hSnapshot, &process);
    }
    return TRUE;
}

void SetPectProcID(DWORD pid)
{
    protectedProcessID = pid;
}

HWND GetCursorWindows()
{
    POINT p;
    GetCursorPos(&p);
    return WindowFromPoint(p);
}


BOOL HookAPI()
{
    LONG res = NO_ERROR;

    orginOpenProcess = DetourFindFunction("kernel32.dll", "OpenProcess");
    orginCreateProcessA = DetourFindFunction("kernel32.dll", "CreateProcessA");
    orginCreateProcessW = DetourFindFunction("kernel32.dll", "CreateProcessW");

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourAttach((PVOID)&orginOpenProcess, (PVOID)HookedOpenProcess);
    DetourAttach((PVOID)&orginCreateProcessA, (PVOID)HookedCreateProcessA);
    DetourAttach((PVOID)&orginCreateProcessW, (PVOID)HookedCreateProcessW);

    DetourTransactionCommit();
    return TRUE;
}

BOOL UnhookAPI()
{
    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());

    DetourDetach((PVOID)&orginOpenProcess, (PVOID)HookedOpenProcess);
    DetourDetach((PVOID)&orginCreateProcessA, (PVOID)HookedCreateProcessA);
    DetourDetach((PVOID)&orginCreateProcessW, (PVOID)HookedCreateProcessW);

    DetourTransactionCommit();
    return TRUE;
}

void SetKeyStatus(DWORD key, BOOL enable)
{
    exceptedKeys[key] = enable;
}

BOOL GetKeyStatus(DWORD key)
{
    return exceptedKeys[key];
}

void ResetExceptedKeys() {
    memset(exceptedKeys, 0, sizeof exceptedKeys);
}