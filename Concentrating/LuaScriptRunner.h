#pragma once

#include <QObject>

struct lua_State;
class QThread;

class LuaScriptRunner : public QObject
{
	Q_OBJECT

public:
	LuaScriptRunner(QObject* parent = Q_NULLPTR);
	~LuaScriptRunner();

	bool isRunning()const { return _running; }

	QString code()const { return _code; }
	void setCode(const QString& code);

	QThread* thread()const { return _thread; }
	void setThread(QThread* thread);

public slots:
	void run();
	void run(const QString& code);

signals:
	void failed(const QString& reason);
	void finished(bool exitCode);

private:
	bool _running;

	QString _code;

	QThread* _thread = nullptr;
};
