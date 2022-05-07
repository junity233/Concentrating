#include "LuaBinding.h"#
#include <Windows.h>
#include <hook.h>
#include <ctime>
#include <lua.hpp>
#include "SettingManager.h"
#include "MainWindow.h"
#include "ProcessHelper.h"
#include <ConcerntratingBrowser.h>
#include <qstring.h>
#include <sys/timeb.h>
#include <qmessagebox.h>
#include <qdatetime.h>

struct lua_enum_pair {
	const char* name;
	int vkcode;
};

#include "vkcodes.inc"
#include "buttons.inc"

static void lua_bind_vkcode(lua_State* L);

static int lua_lock_mouse(lua_State* L);
static int lua_unlock_mouse(lua_State* L);
static int lua_lock_keyboard(lua_State* L);
static int lua_unlock_keyboard(lua_State* L);
static int lua_wait(lua_State* L);
static int lua_wait_until(lua_State* L);
static int lua_set_key_status(lua_State* L);
static int lua_get_key_status(lua_State* L);
static int lua_get_vkcode(lua_State* L);
static int lua_exec(lua_State* L);
static int lua_shutdown(lua_State* L);
static int lua_msgbox(lua_State* L);

static int lua_process_create(lua_State* L);
static int lua_process_find(lua_State* L);
static int lua_process_kill(lua_State* L);

static int lua_setting_read(lua_State* L);
static int lua_setting_write(lua_State* L);

static int lua_browser_open(lua_State* L);
static int lua_browser_close(lua_State* L);
static int lua_browser_set_allowed_hosts(lua_State* L);
static int lua_browser_set_default_url(lua_State* L);
static int lua_browser_get_current_url(lua_State* L);

static void __lua_to_value(lua_State* L,const QVariant& v);
static void __lua_to_map(lua_State* L, const QVariantMap& vmap);
static void __lua_to_list(lua_State* L, const QVariantList& vlst);

static QVariant __lua_from_value(lua_State* L);
static QVariantMap __lua_from_table(lua_State* L);

void lua_bind_vkcode(lua_State* L)
{
	int i = 0;

	lua_newtable(L);

	while (vkcode_enum[i].name != NULL) {
		lua_pushstring(L, vkcode_enum[i].name);
		lua_pushinteger(L, vkcode_enum[i].vkcode);
		lua_rawset(L, -3);

		i++;
	}
	lua_setglobal(L, "Vkcode");
}

void lua_bind_msgboxType(lua_State* L)
{
	int i = 0;

	lua_newtable(L);

	while (msgboxType_enum[i].name != NULL) {
		lua_pushstring(L, msgboxType_enum[i].name);
		lua_pushinteger(L, msgboxType_enum[i].vkcode);
		lua_rawset(L, -3);

		i++;
	}
	lua_setglobal(L, "MsgboxType");
}

int lua_lock_mouse(lua_State* L) {
	int res = InstallMouseHook();
	lua_pushboolean(L, res);
	return 1;
}

int lua_unlock_mouse(lua_State* L) {
	int res = UninstallMouseHook();
	lua_pushboolean(L, res);
	return 1;
}

int lua_lock_keyboard(lua_State * L) {
	int res = InstallKeyboardHook();
	lua_pushboolean(L, res);
	return 1;
}

int lua_unlock_keyboard(lua_State* L) {
	int res = UninstallKeyboardHook();
	lua_pushboolean(L, res);
	return 1;
}

static int lua_log(lua_State* L) {
	int argc = lua_gettop(L);

	lua_pushlstring(L, "", 0);
	for (int i = argc; i >= 1; i--) {
		int idx = -i - 1;
		if (lua_isnumber(L, idx) || lua_isstring(L, idx))
		{
			lua_pushvalue(L, idx);
		}
		else if (lua_isboolean(L, idx)) {
			int res = lua_toboolean(L, idx);
			if (res)
				lua_pushlstring(L, "True", 4);
			else
				lua_pushlstring(L, "False", 5);

		}
		else if (lua_isnil(L, idx))
			lua_pushlstring(L, "Nil", 3);
		else if (lua_istable(L, idx))
			lua_pushlstring(L, "[TABLE]", 7);
		else
			lua_pushvalue(L, idx);

		lua_pushlstring(L, " ", 1);

		lua_concat(L, 3);
	}

	const char* msg = lua_tostring(L, -1);
	lua_pop(L, 1);

	LuaBinder::instance()->callLog(msg);

	return 0;
}

static int lua_wait(lua_State* L) {
	int t = lua_tointeger(L, -1);

	timeb start,tb;
	ftime(&start);

	tb = start;
	while (tb.time * 1000 + tb.millitm < ((start.time + t) * 1000 + start.millitm)) {
		QApplication::processEvents();
		ftime(&tb);
	}

	
	
	return 0;
}

static inline tm* __get_time() {
	time_t t = time(NULL);
	return localtime(&t);

}

static inline bool __time_less_than(tm* t, int h, int m, int s) {
	return t->tm_hour < h&& t->tm_min < m&& t->tm_sec < s;
}

#define check_argument_int(var,idx) \
	{\
		int res=lua_isinteger(L,(idx));\
		if(!res)return luaL_error(L,"Arguments of wait_until should be int!");\
		var=lua_tointeger(L,(idx));\
	}

int lua_wait_until(lua_State* L) {
	int hour, minite = 0, second = 0;
	int res;
	int argc = lua_gettop(L);

	if (argc < 1) {
		return luaL_error(L, "wait_until needs at least one arg!");
	}

	check_argument_int(hour, -3);

	if (argc > 1)
		check_argument_int(minite, -2);

	if (argc > 2)
		check_argument_int(second, -1);

	tm* t = __get_time();

	while (__time_less_than(t, hour, minite, second)) {
		QApplication::processEvents();
	}

	return 0;
}

int lua_set_key_status(lua_State* L)
{
	if (lua_gettop(L) < 2)
		return luaL_error(L, "set_key_status needs at least 2 arguments!");

	if (!lua_isinteger(L, -2))
		return luaL_error(L, "First argumen of set_key_status should be int!");

	if (!lua_isboolean(L, -1))
		return luaL_error(L, "Second argument of set_key_status should be boolean!");


	SetKeyStatus(lua_tointeger(L, -2), lua_toboolean(L, -1));
	return 0;
}

int lua_get_key_status(lua_State* L)
{
	if (lua_gettop(L) < 1)
		return luaL_error(L, "get_key_status needs at least 2 arguments!");

	if (!lua_isinteger(L, -1))
		return luaL_error(L, "Argumen of get_key_status should be int!");

	lua_pushboolean(L, GetKeyStatus(lua_tointeger(L, -1)));
	return 1;
}

int lua_get_vkcode(lua_State* L)
{
	return 0;
}

int lua_exec(lua_State* L)
{
	if (lua_gettop(L) < 1 || !lua_isstring(L, -1))
		return luaL_error(L, "exec function needs one string arg!");

	const char* cmd = lua_tostring(L, -1);
	int res = system(cmd);

	lua_pushinteger(L,res);
	return 1;
}

int lua_shutdown(lua_State* L)
{
	system("shutdown -s -t 0");//TODO
	return 0;
}

int lua_msgbox(lua_State* L)
{
	if (lua_gettop(L) < 3)
		return luaL_error(L, "msgbox needs 3 argument:string,string,number!");

	if (!lua_isstring(L, -3) || !lua_isstring(L, -2) || !lua_isinteger(L, -1))
		return luaL_error(L, "msgbox needs 3 argument:string,string,number!");

	const char* title;
	const char* msg;
	int type;

	title = lua_tostring(L, -3);
	msg = lua_tostring(L, -2);
	type = lua_tointeger(L, -1);
	MB_OK;
	int res = MessageBoxA(NULL, msg, title, type);
	lua_pushinteger(L,res);

	return 1;
}

int lua_process_create(lua_State* L)
{
	int argc = lua_gettop(L);
	if (argc < 1)
		return luaL_error(L, "process.create needs at least 1 argument!");

	else if (argc < 2) {
		if (!lua_isstring(L, -1))
			return luaL_error(L, "Argument of process.create should be string!");

		const char* cmdLine = lua_tostring(L, -1);

		bool res = ProcessHelper::createProcess(cmdLine, Q_NULLPTR);

		lua_pushboolean(L, res);
		return 1;
	}
	else {
		if (!lua_isstring(L, -1)|| !lua_isstring(L, -2))
			return luaL_error(L, "Argument of process.create should be string!");
		const char* cmdLine = lua_tostring(L, -1);
		const char* execPath = lua_tostring(L, -2);
		
		bool res = ProcessHelper::createProcess(execPath, cmdLine);

		lua_pushboolean(L, res);
		return 1;
	}
}

int lua_process_find(lua_State* L)
{
	if (lua_gettop(L) < 1 || !lua_isstring(L, -1))
		return luaL_error(L, "process.find needs 1 argument:string!");

	const char* name = lua_tostring(L, -1);

	auto res = ProcessHelper::findProcess(name);

	lua_newtable(L);

	for (int i = 0; i < res.size(); i++) {
		lua_pushinteger(L, res[i]);

		lua_rawseti(L, -2, i + 1);
	}

	return 1;
}

int lua_process_kill(lua_State* L)
{
	if (lua_gettop(L) < 1 || !lua_isinteger(L, -1))
		return luaL_error(L, "process.kill needs 1 argument:int!");

	int pid = lua_tointeger(L, -1);
	
	lua_pushboolean(L, ProcessHelper::killProcess(pid));

	return 1;
}

void __lua_to_time(lua_State* L, QTime time) {
	lua_newtable(L);

	lua_pushlstring(L, "hour", 4);
	lua_pushinteger(L,time.hour());
	lua_rawset(L,-3);

	lua_pushlstring(L, "minute", 4);
	lua_pushinteger(L, time.minute());
	lua_rawset(L, -3);

	lua_pushlstring(L, "second", 4);
	lua_pushinteger(L, time.second());
	lua_rawset(L, -3);


	lua_pushlstring(L, "__type__", 8);
	lua_pushinteger(L, (int)QVariant::Time);

	lua_rawset(L, -3);
}

void __lua_to_map(lua_State* L, const QVariantMap& vmap) {
	lua_newtable(L);

	for (auto i : vmap.keys()) {
		lua_pushstring(L, i.toStdString().c_str());
		__lua_to_value(L, vmap[i]);

		lua_rawset(L, -3);
	}

	lua_pushlstring(L, "__type__", 8);
	lua_pushinteger(L, (int)QVariant::Map);

	lua_rawset(L, -3);
}
void __lua_to_list(lua_State* L, const QVariantList& vlst) {
	lua_newtable(L);

	for (int i = 0; i < vlst.size(); i++) {
		__lua_to_value(L, vlst[i]);

		lua_rawseti(L, -2, i+1);
	}

	lua_pushlstring(L, "__type__", 8);
	lua_pushinteger(L, (int)QVariant::List);

	lua_rawset(L, -3);
}

QVariant __lua_from_value(lua_State* L)
{
	if (lua_isboolean(L, -1)) {
		return (bool)lua_toboolean(L, -1);
	}
	else if (lua_isstring(L, -1)) {
		return lua_tostring(L, -1);
	}
	else if (lua_isinteger(L, -1)) {
		return lua_tointeger(L, -1);
	}
	else if (lua_isnumber(L, -1)) {
		return lua_tonumber(L, -1);
	}
	else if (lua_istable(L, -1)) {
		QVariantMap map = __lua_from_table(L);

		if (map.contains("__type__")) {
			QTime t;
			switch (map["__type__"].toInt()) {
			case QVariant::Time:
				t.setHMS(map["hour"].toInt(), map["minute"].toInt(), map["second"].toInt());
				return t;
			case QVariant::Map:
			case QVariant::List:
				map.remove("__type__");
				return map;
			}
		}
	}
	return QVariant();
}

QVariantMap __lua_from_table(lua_State* L)
{
	lua_pushnil(L);
	QVariantMap res;

	while (lua_next(L, -2)) {

		lua_pushvalue(L, -2);
		if (!lua_isstring(L, -1))
			luaL_error(L, "can only write string-type-index table to setting!");

		QString key = lua_tostring(L, -1);
		lua_pop(L, 1);

		res.insert(key, __lua_from_value(L));

		lua_pop(L, 1);
	}

	return res;
}

void __lua_to_value(lua_State* L, const QVariant& v) {
	switch (v.type()) {
	case QVariant::String:
		lua_pushstring(L, v.toString().toStdString().c_str());
		break;
	case QVariant::Int:
		lua_pushinteger(L, v.toInt());
	case QVariant::Bool:
		lua_pushboolean(L, v.toBool());
	case QVariant::Double:
		lua_pushnumber(L, v.toDouble());
		break;
	case QVariant::Time:
		__lua_to_time(L, v.toTime());
		break;
	case QVariant::Map:
		__lua_to_map(L, v.toMap());
		break;
	case QVariant::List:
		__lua_to_list(L, v.toList());
	default:
		luaL_error(L, "Unknown type:%d", (int)v.type());
	}
}

int lua_setting_read(lua_State* L)
{
	if (lua_gettop(L) < 1 || !lua_isstring(L, -1))
		return luaL_error(L, "read needs 1 argument:string!");

	QString key = lua_tostring(L, -1);

	if (SettingManager::instance()->keys().contains(key)) {
		QVariant v = SettingManager::instance()->value(key);

		__lua_to_value(L, v);
	}
	else {
		lua_pushnil(L);
	}
	return 1;
}

int lua_setting_write(lua_State* L)
{
	if (lua_gettop(L) < 2||!lua_isstring(L,-2))
		return luaL_error(L, "write needs 2 argument:string,any");

	QVariant v = __lua_from_value(L);
	QString key = lua_tostring(L, -2);

	SettingManager::instance()->setValue(key, v);
	return 0;
}

int lua_browser_open(lua_State* L)
{
	LuaBinder::instance()->openBrowser();

	return 0;
}

int lua_browser_close(lua_State* L)
{
	LuaBinder::instance()->closeBrowser();

	return 0;
}

int lua_browser_set_allowed_hosts(lua_State* L)
{

	if (lua_gettop(L) < 1)
		return luaL_error(L, "set_allowed_hosts needs at least one arg!");

	if (!lua_istable(L, -1))
		return luaL_error(L, "Argumen of set_allowed_hosts should be table!");

	lua_pushnil(L);

	QStringList list;

	while (lua_next(L, -2)) {
		if (lua_isstring(L, -1)) {
			list.append(lua_tostring(L, -1));
		}

		lua_pop(L, 1);
	}

	MainWindow::instance()->browser()->setAllowedHosts(list);
}

int lua_browser_set_default_url(lua_State* L)
{

	if (lua_gettop(L) < 1 || !lua_isstring(L, -1))
		return luaL_error(L, "set_default_url function needs one string arg!");

	QString url = lua_tostring(L, -1);

	MainWindow::instance()->browser()->setDefaultUrl(url);
}

int lua_browser_get_current_url(lua_State* L)
{
	auto url = MainWindow::instance()->browser()->currentUrl().toString().toStdString();
	lua_pushlstring(L, url.c_str(), url.length());
	return 1;
}

#undef check_argument_int

static luaL_Reg concer_functions[] = {
	{"wait",lua_wait},
	{"wait_until",lua_wait_until},
	{"log",lua_log},
	{"exec",lua_exec},
	{"shutdown",lua_shutdown},
	{"msgbox",lua_msgbox},
	{NULL,NULL}
};

static luaL_Reg mouse_functions[] = {
	{"lock",lua_lock_mouse},
	{"unlock",lua_unlock_mouse},
	{NULL,NULL}
};

static luaL_Reg keyboard_functions[] = {
	{"lock",lua_lock_keyboard},
	{"unlock",lua_unlock_keyboard},
	{"get_key_status",lua_get_key_status},
	{"set_key_status",lua_set_key_status},
	{NULL,NULL}
};

static luaL_Reg browser_functions[] = {
	{"open",lua_browser_open},
	{"close",lua_browser_close},
	{"set_allowed_hosts",lua_browser_set_allowed_hosts},
	{"set_default_url",lua_browser_set_default_url},
	{"get_current_url",lua_browser_get_current_url},
	{NULL,NULL}
};

static luaL_Reg setting_functions[] = {
	{"read",lua_setting_read},
	{"write",lua_setting_write},
	{NULL,NULL}
};

static luaL_Reg process_functions[] = {
	{"create",lua_process_create},
	{"find",lua_process_find},
	{"kill",lua_process_kill},
	{NULL,NULL}
};

LuaBinder* LuaBinder::_instance = new LuaBinder;

LuaBinder::LuaBinder() :
	QObject(Q_NULLPTR)
{

}

void LuaBinder::BindLua(lua_State* L)
{
	luaL_openlibs(L);
	lua_bind_vkcode(L);
	lua_bind_msgboxType(L);

	lua_newtable(L);
	luaL_setfuncs(L, concer_functions, 0);


	lua_pushlstring(L, "mouse", 5);
	lua_newtable(L);
	luaL_setfuncs(L, mouse_functions, 0);
	lua_rawset(L, -3);


	lua_pushlstring(L, "keyboard", 8);
	lua_newtable(L);
	luaL_setfuncs(L, keyboard_functions, 0);
	lua_rawset(L, -3);

	lua_pushlstring(L, "browser", 7);
	lua_newtable(L);
	luaL_setfuncs(L, browser_functions, 0);
	lua_rawset(L, -3);

	lua_pushlstring(L, "setting", 7);
	lua_newtable(L);
	luaL_setfuncs(L, setting_functions, 0);
	lua_rawset(L, -3);

	lua_pushlstring(L, "process", 7);
	lua_newtable(L);
	luaL_setfuncs(L, process_functions, 0);
	lua_rawset(L, -3);

	lua_setglobal(L, "Concer");
}

void LuaBinder::callLog(const QString& msg)
{
	_logCallback(msg);
}

void LuaBinder::setLogCallback(LuaLogCallback callback)
{
	LuaBinder::_logCallback = callback;
}