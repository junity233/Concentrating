#pragma once

#include <QObject>

struct lua_State;

class LuaScriptRunner : public QObject
{
	Q_OBJECT

public:
	LuaScriptRunner(QObject* parent = Q_NULLPTR);
	LuaScriptRunner(lua_State* L, QObject* parent = Q_NULLPTR);
	~LuaScriptRunner();

	bool isRunning()const { return running; }

public slots:
	void run(int index);

signals:
	void scriptRunFailed(const QString& reason);
	void scriptRunFinished(bool exitCode);

private:
	bool running;
};
