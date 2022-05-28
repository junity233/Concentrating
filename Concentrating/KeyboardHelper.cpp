#include "KeyboardHelper.h"
#include <hook.h>
#include <qthread.h>

bool KeyboardHelper::_isLocked = false;

QMutex KeyboardHelper::_mutex;

bool KeyboardHelper::lock()
{
    if (!_isLocked) {
        _isLocked = true;
        _mutex.lock();
        bool res = InstallKeyboardHook();
        _mutex.unlock();

        return res;
    }
    return true;
}

bool KeyboardHelper::unlock()
{
    if (_isLocked) {
        _isLocked = false;
        _mutex.lock();
        bool res = UninstallKeyboardHook();
        _mutex.unlock();

        return res;
    }
    return true;
}

void KeyboardHelper::setKeyState(int key, bool enable)
{
    SetKeyStatus(key, enable);
}

bool KeyboardHelper::keyState(int key)
{
    return GetKeyState(key);
}
