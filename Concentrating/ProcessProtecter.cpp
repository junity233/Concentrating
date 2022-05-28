#include "ProcessProtecter.h"
#include <hook.h>

bool ProcessProtecter::_protected = false;

bool ProcessProtecter::protect(int pid)
{
    _protected = true;
    SetPectProcID(pid);
    return InstallProcPectHook();
}

bool ProcessProtecter::unprotect()
{
    _protected = false;
    SetPectProcID(NULL);
    return UninstallProcPectHook();
}

bool ProcessProtecter::isProtected()
{
    return _protected;
}
