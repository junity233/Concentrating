#include "LuaScriptRunner.h"
#include <lua.hpp>
#include "ScriptManager.h"

#include "LuaBinding.h"

LuaScriptRunner::LuaScriptRunner(QObject *parent)
	: QObject(parent)
{
	running = false;
}

LuaScriptRunner::LuaScriptRunner(lua_State* L, QObject* parent)
	:QObject(parent)
{
	running = false;
}

LuaScriptRunner::~LuaScriptRunner()
{
}

void LuaScriptRunner::run(int index) {
	lua_State* L = luaL_newstate();
	LuaBinder::BindLua(L);

	if (index >= ScriptManager::instance()->scriptCount()) {
		emit scriptRunFailed(tr("The specified index does not exist"));
		return;
	}

	if (running) {
		emit scriptRunFailed(tr("A script is already running"));
		return;
	}

	QString script = ScriptManager::instance()->scripts()[index].code;
	if (script.isNull()) {
		emit scriptRunFailed(tr("The script is null!"));
		return;
	}

	running = true;
	int res = luaL_dostring(L, script.toStdString().c_str());
	running = false;

	if (res != 0) {
		QString reason = lua_tostring(L, -1);
		emit scriptRunFailed(reason);
	}

	lua_close(L);

	emit scriptRunFinished(res);
}