#pragma once

#include <QObject>
#include <qvector.h>
#include <qthread.h>

#include "LuaScriptRunner.h"

class LuaScriptRunnerPool : public QObject
{
	Q_OBJECT

public:
	LuaScriptRunnerPool(QObject *parent);
	~LuaScriptRunnerPool();

	struct Task {
		QString name;
		QString code;
		LuaScriptRunner* runner;
		QThread* thread;

		int id;

		bool operator ==(const Task& b) {
			return id == b.id;
		}
	};

	int count()const { return _tasks.count(); }

	bool isFinished()const { return count() == 0; }

	QVector<Task> tasks()const { return _tasks; }

public slots:

	void run(const QString& name, const QString& code);

signals:
	void failed(const QString& name,const QString& reason);
	void finished(const QString& name, bool exitCode);

private:
	QVector<Task> _tasks;
};