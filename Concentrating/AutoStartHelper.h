#pragma once

class QString;

class AutoStartHelper
{
public:
	static void setAutoStartEnable(bool state);
	static bool isAutoStartEnable();

	/**
	 * @brief �жϳ����Ƿ����Զ�������
	 * @return 
	*/
	static bool isApplicationAutoStart();
private:
	static QString regPath;
	static QString key;
};

