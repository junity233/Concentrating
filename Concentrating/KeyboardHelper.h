#pragma once
class KeyboardHelper
{
public:
	static bool lock();
	static bool unlock();
	static void setKeyState(int key, bool enable);
	static bool keyState(int key);
};

