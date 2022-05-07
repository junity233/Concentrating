#pragma once

#include <functional>
#include <qstring.h>
#include <qobject.h>

struct lua_State;
class ConcerntratingBrowser;

class LuaBinder :
	public QObject
{
	Q_OBJECT
	LuaBinder();
public:
	typedef std::function<void(const QString& msg)> LuaLogCallback;

	static void  BindLua(struct lua_State* L);

	void callLog(const QString& msg);
	void setLogCallback(LuaLogCallback callback);

	static LuaBinder* instance() { return _instance; }

signals:
	void openBrowser();
	void closeBrowser();

private:
	LuaLogCallback _logCallback;
	static LuaBinder* _instance;
};