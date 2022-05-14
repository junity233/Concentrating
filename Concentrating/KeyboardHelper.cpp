#include "KeyboardHelper.h"
#include <hook.h>

bool KeyboardHelper::lock()
{
    return InstallKeyboardHook();
}

bool KeyboardHelper::unlock()
{
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
