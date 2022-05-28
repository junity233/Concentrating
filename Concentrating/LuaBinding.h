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

	static void  BindLua(struct lua_State* L);

	static LuaBinder* instance() { return _instance; }

signals:
	void openBrowser();
	void closeBrowser();
	void browserLoadUrl(const QUrl& url);
	void lockMouse();
	void lockKeyboard();
	void unlockMouse();
	void unlockKeyboard();

	void log(const QString& msg);

	void say(const QString& text);

private:
	static LuaBinder* _instance;
};