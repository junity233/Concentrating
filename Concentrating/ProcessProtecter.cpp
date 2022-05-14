#include "ProcessProtecter.h"
#include <hook.h>

bool ProcessProtecter::_protected = false;

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

bool ProcessProtecter::isProtected()
{
    return _protected;
}
