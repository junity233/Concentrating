#include "KeyboardHelper.h"
#include <hook.h>

bool KeyboardHelper::_isLocked = false;

bool KeyboardHelper::lock()
{
    _isLocked = true;
    return InstallKeyboardHook();
}

bool KeyboardHelper::unlock()
{
    _isLocked = false;
    return UninstallKeyboardHook();
}

void KeyboardHelper::setKeyState(int key, bool enable)
{
    ::SetKeyStatus(key, enable);
}

bool KeyboardHelper::keyState(int key)
{
    return GetKeyState(key);
}
