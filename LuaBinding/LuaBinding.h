#pragma once

#include <functional>
#include <qstring.h>

struct lua_State;
class ConcerntratingBrowser;

class LuaBinder
{
	LuaBinder() = delete;
public:
	typedef std::function<void(const QString& msg)> LuaLogCallback;

	static void  BindLua(struct lua_State* L);

	static void callLog(const QString& msg);
	static void setLogCallback(LuaLogCallback callback);

	static void initBrowser();

	static ConcerntratingBrowser* browser() { return _browser; }

private:
	static LuaLogCallback _logCallback;
	static ConcerntratingBrowser* _browser;
};