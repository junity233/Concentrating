#include "MouseHelper.h"
#include <hook.h>
#include <Windows.h>

bool MouseHelper::_isLocked = false;

bool MouseHelper::lock()
{
    if (!_isLocked) {
        _isLocked = true;
        return InstallMouseHook();
    }
    return true;
}

bool MouseHelper::unlock()
{
    if (_isLocked) {
        _isLocked = false;
        return UninstallMouseHook();
    }
    return true;
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
