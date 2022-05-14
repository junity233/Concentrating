#pragma once

class QString;

class AutoStartHelper
{
public:
	static void setAutoStartEnable(bool state);
	static bool isAutoStartEnable();

	/**
	 * @brief 判断程序是否是自动启动的
	 * @return 
	*/
	static bool isApplicationAutoStart();
private:
	static QString regPath;
	static QString key;
};

