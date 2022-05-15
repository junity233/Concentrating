#include "LuaScriptRunner.h"
#include <lua.hpp>
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

	auto browser = MainWindow::instance()->browser();

	browser->resetAllowedHosts();
	browser->resetDefaultUrl();

	browser->setDefaultUrl(
		QUrl::fromUserInput(
			SettingManager::instance()->value("browser.default_page_url", "about:blank").toString()
		)
	);

	running = true;
	bool res = luaL_dostring(L, script.toStdString().c_str());
	running = false;

	if (MouseHelper::isLocked())
		MouseHelper::unlock();

	if (KeyboardHelper::isLocked())
		KeyboardHelper::unlock();

	if (res != 0) {
		QString reason = lua_tostring(L, -1);
		emit scriptRunFailed(reason);
	}

	lua_close(L);

	emit scriptRunFinished(res);
}