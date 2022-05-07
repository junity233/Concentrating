#include "ProcessProtecter.h"
#include <hook.h>

bool ProcessProtecter::protect(int pid)
{
    SetPectProcID(pid);
    return InstallProcPectHook();
}

bool ProcessProtecter::unprotect()
{
    SetPectProcID(NULL);
    return UninstallProcPectHook();
}
