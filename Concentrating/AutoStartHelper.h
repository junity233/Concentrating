#pragma once

class QString;

class AutoStartHelper
{
public:
	static void setAutoStart(bool state);
	static bool isAutoStart();

private:
	static QString regPath;
	static QString key;

};

