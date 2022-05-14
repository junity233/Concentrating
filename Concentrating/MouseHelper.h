#pragma once
#include <qpair.h>

class MouseHelper
{
public:
	static bool lock();
	static bool unlock();
	static QPair<int, int> pos();
	static void move(int x, int y);
};

