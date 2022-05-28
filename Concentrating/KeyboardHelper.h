#pragma once

#include <qmutex.h>

class QThread;

class KeyboardHelper
{
public:
	static bool lock();
	static bool unlock();
	static void setKeyState(int key, bool enable);
	static bool keyState(int key);

	static bool isLocked() { return _isLocked; }

private:
	static bool _isLocked;
	static QMutex _mutex;
};

