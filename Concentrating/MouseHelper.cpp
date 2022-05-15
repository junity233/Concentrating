#include "MouseHelper.h"
#include <hook.h>
#include <Windows.h>

bool MouseHelper::_isLocked = false;

bool MouseHelper::lock()
{
    _isLocked = true;
    return InstallMouseHook();
}

bool MouseHelper::unlock()
{
    _isLocked = false;
    return UninstallMouseHook();
}

QPair<int, int> MouseHelper::pos()
{
    POINT res;
    GetCursorPos(&res);

    return qMakePair(res.x, res.y);
}

void MouseHelper::move(int x, int y)
{
    SetCursorPos(x, y);
}
