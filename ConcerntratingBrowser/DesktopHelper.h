#pragma once
#include <qstring.h>

struct HDESK__;

class DesktopHelper
{
public:

	class Desktop {
	public:
		Desktop(HDESK__* hDesk);

		QString name();
		bool switchTo();
	private:
		HDESK__* _hDesk;
	};

	static Desktop GetCurrentDesktop();
	static Desktop GetThreadDesktop();
};

