#include "LuaScriptRunner.h"
#include <lua.hpp>
#include <qthread.h>
#include <qdebug.h>

#include "ScriptManager.h"
#include "MainWindow.h"
#include "ConcerntratingBrowser.h"
#include "SettingManager.h"
#include "LuaBinding.h"
#include "MouseHelper.h"
#include "KeyboardHelper.h"

LuaScriptRunner::LuaScriptRunner(QObject *parent)
	: QObject(parent)
{
	_running = false;
}

LuaScriptRunner::~LuaScriptRunner()
{
}

void LuaScriptRunner::setCode(const QString& code)
{
	_code = code;
}

void LuaScriptRunner::setThread(QThread* thread)
{
	_thread = thread;
	moveToThread(_thread);
}

void LuaScriptRunner::run(const QString& code)
{
	setCode(code);
	run();
}

void LuaScriptRunner::run() {
	lua_State* L = luaL_newstate();
	LuaBinder::BindLua(L);

	if (_running) {
		emit failed(tr("A script is already running"));
		return;
	}

	if (_code.isNull()) {
		emit failed(tr("The script is null!"));
		return;
	}

	_running = true;
	bool res = luaL_dostring(L, _code.toStdString().c_str());
	_running = false;

	if (res != 0) {
		QString reason = lua_tostring(L, -1);
		emit failed(reason);
	}

	lua_close(L);

	emit finished(res);
}