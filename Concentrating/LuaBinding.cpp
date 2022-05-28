#include "LuaBinding.h"
#include <Windows.h>
#include <hook.h>
#include <ctime>
#include <lua.hpp>
#include <ConcerntratingBrowser.h>
#include <qstring.h>
#include <sys/timeb.h>
#include <qmessagebox.h>
#include <qdatetime.h>
#include <qsystemtrayicon.h>

#include "MouseHelper.h"
#include "KeyboardHelper.h"
#include "ScriptManager.h"
#include "ShutdownHelper.h"
#include "SettingManager.h"
#include "MainWindow.h"
#include "ProcessHelper.h"
#include "MusicPlayer.h"
#include "TextSpeaker.h"

struct lua_enum_pair {
	const char* name;
	int vkcode;
};

#include "vkcodes.inc"
#include "buttons.inc"

static void lua_bind_vkcode(lua_State* L);

static int lua_exec(lua_State* L);
static int lua_shutdown(lua_State* L);
static int lua_log(lua_State* L);
static int lua_msgbox(lua_State* L);
static int lua_wait(lua_State* L);
static int lua_wait_until(lua_State* L);
static int lua_make_time(lua_State* L);
static int lua_message(lua_State* L);
static int lua_run_script(lua_State* L);
static int lua_process_event(lua_State* L);
static int lua_beep(lua_State* L);
static int lua_import(lua_State* L);

static int lua_speaker_say(lua_State* L);
static int lua_speaker_volume(lua_State* L);
static int lua_speaker_set_volume(lua_State* L);
static int lua_speaker_rate(lua_State* L);
static int lua_speaker_set_rate(lua_State* L);
static int lua_speaker_stop(lua_State* L);
static int lua_speaker_pause(lua_State* L);
static int lua_speaker_resume(lua_State* L);

static int lua_mouse_lock(lua_State* L);
static int lua_mouse_unlock(lua_State* L);
static int lua_mouse_pos(lua_State* L);
static int lua_mouse_move(lua_State* L);

static int lua_keyboard_lock(lua_State* L);
static int lua_keyboard_unlock(lua_State* L);
static int lua_keyboard_set_key_status(lua_State* L);
static int lua_keyboard_get_key_status(lua_State* L);
static int lua_keyboard_enable_keys(lua_State* L);
static int lua_keyboard_disable_keys(lua_State* L);

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
static int lua_browser_load(lua_State* L);

static int lua_media_play(lua_State* L);
static int lua_media_play_background(lua_State* L);
static int lua_media_volume(lua_State* L);
static int lua_media_background_volume(lua_State* L);
static int lua_media_set_volume(lua_State* L);
static int lua_media_set_background_volume(lua_State* L);

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
	lua_setglobal(L, "Msgbox");
}

int lua_mouse_lock(lua_State* L) {
	emit LuaBinder::instance()->lockMouse();
	return 0;
}

int lua_mouse_unlock(lua_State* L) {
	emit LuaBinder::instance()->unlockMouse();
	return 0;
}

int lua_mouse_pos(lua_State* L)
{
	auto pos = MouseHelper::pos();

	lua_pushinteger(L, pos.first);
	lua_pushinteger(L, pos.second);

	return 2;
}

int lua_mouse_move(lua_State* L)
{
	int x, y;
	x = luaL_checkinteger(L, 1);
	y = luaL_checkinteger(L, 2);

	MouseHelper::move(x, y);

	return 0;
}

int lua_keyboard_lock(lua_State * L) {
	emit LuaBinder::instance()->lockKeyboard();
	return 0;
}

int lua_keyboard_unlock(lua_State* L) {
	emit LuaBinder::instance()->unlockKeyboard();
	return 0;
}

int lua_log(lua_State* L) {
	int argc = lua_gettop(L);
	QString t;

	lua_pushlstring(L, "", 0);

	for (int i = 1; i <= argc; i++) {

		switch (lua_type(L, i)) {
		case LUA_TSTRING:
		case LUA_TNUMBER:
			lua_pushvalue(L, i);
			break;
		case LUA_TTABLE:
			lua_pushlstring(L, "[TABLE]", 7);
			break;
		case LUA_TNIL:
			lua_pushlstring(L, "nil", 3);
			break;
		case LUA_TBOOLEAN:
			if(lua_toboolean(L,i))
				lua_pushlstring(L, "True", 4);
			else
				lua_pushlstring(L, "False", 5);
			break;
		}

		lua_pushlstring(L, " ", 1);
		lua_concat(L, 3);
	}

	const char* msg = lua_tostring(L, -1);

	emit LuaBinder::instance()->log(msg);

	lua_pop(L, 1);

	return 0;
}

static int lua_wait(lua_State* L) {
	int t = luaL_checkinteger(L, -1);

	Sleep(t);

	return 0;
}

static inline tm* __get_time() {
	time_t t = time(NULL);
	return localtime(&t);

}

static inline bool __time_less_than(tm* t, int h, int m, int s) {
	if (t->tm_hour < h)
		return true;
	else if(t->tm_hour == h) {
		if (t->tm_min < m)
			return true;
		else if (t->tm_min == m && t->tm_sec < s)
			return true;
	}
	return false;
}


int lua_wait_until(lua_State* L) {
	int hour, minite = 0, second = 0;

	hour = luaL_checkinteger(L, 1);
	minite = lua_tointeger(L, 2);
	second = lua_tointeger(L, 3);

	tm* t = __get_time();

	while (__time_less_than(t, hour, minite, second)) {
		Sleep(100);
		t = __get_time();
	}

	return 0;
}

int lua_make_time(lua_State* L)
{
	int hour, minite = 0, second = 0;
	int res;
	int argc = lua_gettop(L);

	hour = luaL_checkinteger(L, 1);
	minite = luaL_checkinteger(L, 2);
	second = luaL_checkinteger(L, 3);

	lua_newtable(L);

	lua_pushlstring(L, "hour", 4);
	lua_pushinteger(L, hour);
	lua_rawset(L, -3);

	lua_pushlstring(L, "min", 4);
	lua_pushinteger(L, minite);
	lua_rawset(L, -3);

	lua_pushlstring(L, "sec", 4);
	lua_pushinteger(L, second);
	lua_rawset(L, -3);

	return 1;
}

int lua_message(lua_State* L)
{
	auto systemTray = MainWindow::instance()->systemTray();

	QString title = luaL_checkstring(L, 1);
	QString msg = luaL_checkstring(L, 2);

	systemTray->showMessage(title, msg);

	return 0;
}

int lua_run_script(lua_State* L)
{
	QString script;
	auto instance = ScriptManager::instance();

	if (lua_isinteger(L, 1)) {
		int idx = lua_tointeger(L, 1);

		if (idx >= 0 && idx < instance->scriptCount())
			script = instance->script(idx).code;
		else
			return luaL_error(L, "index out of range!");
	}
	else if (lua_isstring(L, 1)) {
		QString name = lua_tostring(L, 1);
		bool has = false;

		for (auto i : ScriptManager::instance()->scripts())
			if (i.name == name) {
				script = i.code;
				has = true;
				break;
			}

		if (!has)
			return luaL_error(L, "No such script!");
	}
	else return luaL_argerror(L, 1,"Need to be int or string");

	luaL_dostring(L, script.toStdString().c_str());

	return 0;
}

int lua_process_event(lua_State* L)
{
	QApplication::processEvents();
	return 0;
}

int lua_beep(lua_State* L)
{
	int freq = luaL_checkinteger(L, 1);
	int dur = luaL_checkinteger(L, 2);

	lua_pushboolean(L, Beep(freq, dur));

	return 1;
}

int lua_import(lua_State* L)
{
	lua_getglobal(L, "Concen");

	lua_pushnil(L);

	while (lua_next(L, -2)) {
		lua_pushvalue(L, -2);
		lua_pushvalue(L, -2);
		lua_setglobal(L, lua_tostring(L, -2));
		lua_pop(L, 2);
	}

	lua_pop(L, 1);

	return 0;
}

int lua_speaker_say(lua_State* L)
{
	QString text = luaL_checkstring(L, 1);
	TextSpeaker::say(text);

	return 0;
}

int lua_speaker_volume(lua_State* L)
{
	lua_pushinteger(L, TextSpeaker::volume());

	return 1;
}

int lua_speaker_set_volume(lua_State* L)
{
	TextSpeaker::setVolume(luaL_checkinteger(L, 1));

	return 0;
}

int lua_speaker_rate(lua_State* L)
{
	lua_pushinteger(L, TextSpeaker::rate());

	return 1;
}

int lua_speaker_set_rate(lua_State* L)
{
	TextSpeaker::setRate(luaL_checkinteger(L, 1));

	return 0;
}

int lua_speaker_stop(lua_State* L)
{
	TextSpeaker::stop();
	return 0;
}

int lua_speaker_pause(lua_State* L)
{
	TextSpeaker::pause();
	return 0;
}

int lua_speaker_resume(lua_State* L)
{
	TextSpeaker::resume();

	return 0;
}

int lua_keyboard_set_key_status(lua_State* L)
{
	luaL_checktype(L, 2, LUA_TBOOLEAN);
	KeyboardHelper::setKeyState(luaL_checkinteger(L, 1), lua_toboolean(L, 2));
	return 0;
}

int lua_keyboard_get_key_status(lua_State* L)
{
	lua_pushboolean(L, KeyboardHelper::keyState(luaL_checkinteger(L, 1)));
	return 1;
}

int lua_keyboard_enable_keys(lua_State* L)
{
	luaL_checkany(L, 1);

	QVector<int> keys;

	if (lua_istable(L, 1)) {
		lua_pushnil(L);

		while (lua_next(L, -1)) {
			if (!lua_isinteger(L, -1))
				return luaL_argerror(L, 1, "Member of list should be int!");

			keys.append(lua_tointeger(L, -1));

			lua_pop(L, 1);
		}
	}
	else {
		int cnt = lua_gettop(L);

		for (int i = 1; i <= cnt; i++)
		{
			keys.append(luaL_checkinteger(L, i));
		}
	}

	for (auto i : keys) {
		KeyboardHelper::setKeyState(i, true);
	}

	return 0;
}

int lua_keyboard_disable_keys(lua_State* L)
{
	luaL_checkany(L, 1);

	QVector<int> keys;

	if (lua_istable(L, 1)) {
		lua_pushnil(L);

		while (lua_next(L, -1)) {
			if (!lua_isinteger(L, -1))
				return luaL_argerror(L, 1, "Member of list should be int!");

			keys.append(lua_tointeger(L, -1));

			lua_pop(L, 1);
		}
	}
	else {
		int cnt = lua_gettop(L);

		for (int i = 1; i <= cnt; i++)
		{
			keys.append(luaL_checkinteger(L, i));
		}
	}

	for (auto i : keys) {
		KeyboardHelper::setKeyState(i, false);
	}

	return 0;
}

int lua_exec(lua_State* L)
{
	const char* cmd = luaL_checkstring(L, 1);
	int res = system(cmd);

	lua_pushinteger(L,res);
	return 1;
}

int lua_shutdown(lua_State* L)
{
	lua_pushboolean(L, ShutdownHelper::shutdown());
	return 1;
}

int lua_msgbox(lua_State* L)
{
	QString title;
	QString msg;
	int type;

	title = luaL_checkstring(L, 1);
	msg = luaL_checkstring(L, 2);
	type = luaL_checkinteger(L, 3);

	int res = MessageBoxA(NULL, msg.toLocal8Bit().constData(), title.toLocal8Bit().constData(), type);
	lua_pushinteger(L,res);

	return 1;
}

int lua_process_create(lua_State* L)
{
	const char* execPath = luaL_checkstring(L, 1);
	const char* cmdLine = lua_tostring(L, 2);

	if (cmdLine == nullptr)
		std::swap(execPath, cmdLine);

	int pid = ProcessHelper::createProcess(execPath, cmdLine);

	lua_pushinteger(L, pid);
	return 1;
}

int lua_process_find(lua_State* L)
{
	const char* name = luaL_checkstring(L, 1);

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
	int pid = luaL_checkinteger(L, 1);
	
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
		return luaL_checkinteger(L, -1);
	}
	else if (lua_isnumber(L, -1)) {
		return lua_tonumber(L, -1);
	}
	else if (lua_istable(L, -1)) {
		QVariantMap map = __lua_from_table(L);

		return map;
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
	QString key = luaL_checkstring(L, 1);

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
	QVariant v = __lua_from_value(L);
	QString key = luaL_checkstring(L, 1);

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
	luaL_checktype(L, 1, LUA_TTABLE);
	lua_pushnil(L);

	QStringList list;

	while (lua_next(L, -2)) {
		if (lua_isstring(L, -1)) {
			list.append(lua_tostring(L, -1));
		}

		lua_pop(L, 1);
	}

	MainWindow::instance()->browser()->setAllowedHosts(list);

	return 0;
}

int lua_browser_set_default_url(lua_State* L)
{
	QString url = luaL_checkstring(L, 1);

	MainWindow::instance()->browser()->setDefaultUrl(QUrl::fromUserInput(url));

	return 0;
}

int lua_browser_get_current_url(lua_State* L)
{
	auto url = MainWindow::instance()->browser()->currentUrl().toString().toStdString();
	lua_pushlstring(L, url.c_str(), url.length());
	return 1;
}

int lua_browser_load(lua_State* L)
{
	QString url = luaL_checkstring(L, 1);
	LuaBinder::instance()->browserLoadUrl(QUrl::fromUserInput(url));

	return 0;
}

int lua_media_play(lua_State* L)
{
	QString name = luaL_checkstring(L, 1);
	MusicPlayer::playEffect(name);

	return 0;
}

static luaL_Reg concen_functions[] = {
	{"wait",lua_wait},
	{"wait_until",lua_wait_until},
	{"log",lua_log},
	{"exec",lua_exec},
	{"shutdown",lua_shutdown},
	{"msgbox",lua_msgbox},
	{"message",lua_message},
	{"run_script",lua_run_script},
	{"process_event",lua_process_event},
	{"make_time",lua_make_time},
	{"beep",lua_beep},
	{"import",lua_import},
	{NULL,NULL}
};

static luaL_Reg mouse_functions[] = {
	{"lock",lua_mouse_lock},
	{"unlock",lua_mouse_unlock},
	{"pos",lua_mouse_pos},
	{"move",lua_mouse_move},
	{NULL,NULL}
};

static luaL_Reg keyboard_functions[] = {
	{"lock",lua_keyboard_lock},
	{"unlock",lua_keyboard_unlock},
	{"get_key_status",lua_keyboard_get_key_status},
	{"set_key_status",lua_keyboard_set_key_status},
	{"enable_keys",lua_keyboard_enable_keys},
	{"disable_keys",lua_keyboard_disable_keys},
	{NULL,NULL}
};

static luaL_Reg browser_functions[] = {
	{"open",lua_browser_open},
	{"close",lua_browser_close},
	{"load",lua_browser_load},
	{"set_allowed_hosts",lua_browser_set_allowed_hosts},
	{"set_default_url",lua_browser_set_default_url},
	{"current_url",lua_browser_get_current_url},
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

static luaL_Reg media_functions[] = {
	{"play",lua_media_play},
	{NULL,NULL}
};

static luaL_Reg speaker_functions[] = {
	{"say",lua_speaker_say},
	{"volume",lua_speaker_volume},
	{"set_volume",lua_speaker_set_volume},
	{"rate",lua_speaker_rate},
	{"set_rate",lua_speaker_set_rate},
	{"pause",lua_speaker_pause},
	{"stop",lua_speaker_stop},
	{"resume",lua_speaker_resume},
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

	luaL_newlib(L, concen_functions);

	lua_pushlstring(L, "mouse", 5);
	luaL_newlib(L, mouse_functions);
	lua_rawset(L, -3);

	lua_pushlstring(L, "keyboard", 8);
	luaL_newlib(L, keyboard_functions);
	lua_rawset(L, -3);

	lua_pushlstring(L, "browser", 7);
	luaL_newlib(L, browser_functions);
	lua_rawset(L, -3);

	lua_pushlstring(L, "setting", 7);
	luaL_newlib(L, setting_functions);
	lua_rawset(L, -3);

	lua_pushlstring(L, "process", 7);
	luaL_newlib(L, process_functions);
	lua_rawset(L, -3);

	lua_pushlstring(L, "speaker", 7);
	luaL_newlib(L, speaker_functions);
	lua_rawset(L, -3);

	lua_setglobal(L, "Concen");

	lua_pushcfunction(L, lua_log);
	lua_setglobal(L, "print");
}