#include "LuaBinding.h"

#include <Windows.h>
#include <hook.h>
#include <ctime>

#include <lua.hpp>

#include <ConcerntratingBrowser.h>
#include <qstring.h>

struct _vkcode_pair {
	const char* name;
	int vkcode;
};

#include "vkcodes.inc"

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
static int lua_browser_open(lua_State* L);
static int lua_browser_close(lua_State* L);
static int lua_Browser_set_allowed_hosts(lua_State* L);

void lua_bind_vkcode(lua_State* L)
{
	int i = 0;

	lua_newtable(L);

	while (vkcode_pairs[i].name != NULL) {
		lua_pushstring(L, vkcode_pairs[i].name);
		lua_pushinteger(L, vkcode_pairs[i].vkcode);
		lua_rawset(L, -3);

		i++;
	}
	lua_setglobal(L, "Vkcode");
}

static int lua_lock_mouse(lua_State* L) {
	int res = InstallMouseHook();
	lua_pushboolean(L, res);
	return 1;
}

static int lua_unlock_mouse(lua_State* L) {
	int res = UninstallMouseHook();
	lua_pushboolean(L, res);
	return 1;
}

static int lua_lock_keyboard(lua_State* L) {
	int res = InstallKeyboardHook();
	lua_pushboolean(L, res);
	return 1;
}

static int lua_unlock_keyboard(lua_State* L) {
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

	LuaBinding::callLog(msg);

	return 0;
}

static int lua_wait(lua_State* L) {
	int time = lua_tointeger(L, -1);

	Sleep(time * 1000);
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

static int lua_wait_until(lua_State* L) {
	int hour, minite = 0, second = 0;
	int res;
	int argc = lua_gettop(L);

	if (argc < 1) {
		return luaL_error(L, "wait_until needs at least one arg!");
	}

	check_argument_int(hour, -1);

	if (argc > 1)
		check_argument_int(minite, -2);

	if (argc > 2)
		check_argument_int(second, -3);

	tm* t = __get_time();

	while (__time_less_than(t, hour, minite, second)) {
		Sleep(1000);
		t = __get_time();
	}

	return 0;
}

int lua_set_key_status(lua_State* L)
{
	if (lua_gettop(L) < 2)
		return luaL_error(L, "set_key_status needs at least 2 arguments!");

	if (!lua_isinteger(L, -1))
		return luaL_error(L, "First argumen of set_key_status should be int!");

	if (!lua_isboolean(L, -1))
		return luaL_error(L, "Second argument of set_key_status should be int!");


	SetKeyStatus(lua_tointeger(L, -1), lua_toboolean(L, -2));
	return 0;
}

int lua_get_key_status(lua_State* L)
{
	if (lua_gettop(L) < 1)
		return luaL_error(L, "get_key_status needs at least 2 arguments!");

	if (!lua_isinteger(L, -1))
		return luaL_error(L, "First argumen of get_key_status should be int!");

	lua_pushboolean(L, GetKeyStatus(lua_tointeger(L, -1)));
	return 1;
}

int lua_get_vkcode(lua_State* L)
{
	return 0;
}

int lua_browser_open(lua_State* L)
{
	if (!LuaBinding::browser())
		LuaBinding::initBrowser();
	LuaBinding::browser()->show();

	return 0;
}

int lua_browser_close(lua_State* L)
{
	if (LuaBinding::browser())
		LuaBinding::browser()->hide();

	return 0;
}

int lua_Browser_set_allowed_hosts(lua_State* L)
{
	return 0;
}

#undef check_argument_int

static luaL_Reg concer_functions[] = {
	{"lua_lock_mouse",lua_lock_mouse},
	{"unlock_mouse",lua_unlock_mouse},
	{"lock_keyboard",lua_lock_keyboard},
	{"unlock_keyboard",lua_unlock_keyboard},
	{"get_key_status",lua_get_key_status},
	{"set_key_status",lua_set_key_status},
	{"wait",lua_wait},
	{"wait_until",lua_wait_until},
	{"log",lua_log},
	{NULL,NULL}
};

static luaL_Reg browser_functions[] = {
	{"open",lua_browser_open},
	{"close",lua_browser_close},
	{"set_allowed_hosts",lua_Browser_set_allowed_hosts},
	{NULL,NULL}
};

LuaBinding::LuaLogCallback LuaBinding::_logCallback = LuaLogCallback();
ConcerntratingBrowser* LuaBinding::_browser = Q_NULLPTR;

void LuaBinding::BindLua(lua_State* L)
{
	luaL_openlibs(L);
	lua_bind_vkcode(L);

	lua_newtable(L);
	luaL_setfuncs(L, concer_functions, 0);

	lua_pushlstring(L, "_browser", 7);
	lua_newtable(L);
	luaL_setfuncs(L, browser_functions, 0);

	lua_rawset(L, -3);

	lua_setglobal(L, "Concer");
}

void LuaBinding::callLog(const QString& msg)
{
	_logCallback(msg);
}

void LuaBinding::setLogCallback(LuaLogCallback callback)
{
	LuaBinding::_logCallback = callback;
}

void LuaBinding::initBrowser()
{
	_browser = new ConcerntratingBrowser;
}
