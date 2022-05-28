#include "LuaScriptRunnerPool.h"
#include "MethodInvoker.hpp"


LuaScriptRunnerPool::LuaScriptRunnerPool(QObject *parent)
	: QObject(parent)
{
}

LuaScriptRunnerPool::~LuaScriptRunnerPool()
{
}

void LuaScriptRunnerPool::run(const QString& name, const QString& code) {
	QThread* thread = new QThread(this);
	thread->setObjectName(QStringLiteral("Script ") + name + QStringLiteral(" thread"));
	thread->start();

	LuaScriptRunner* runner = new LuaScriptRunner();
	runner->setCode(code);
	runner->setThread(thread);

	Task task{ name,code,runner,thread ,rand() };
	_tasks.append(task);

	connect(runner, &LuaScriptRunner::failed, [this,task](const QString& reason) {
		emit failed(task.name, reason);
		});
	connect(runner, &LuaScriptRunner::finished, [this, task](bool exitCode) {
		emit finished(task.name, exitCode);
		task.thread->quit();
		});

	connect(thread, &QThread::finished, [task,this]() {
		task.thread->deleteLater();
		task.runner->deleteLater();

		_tasks.removeOne(task);
		});

	MethodInvoker invoker(this);

	connect(&invoker, &MethodInvoker::invoke, runner, qOverload<>(&LuaScriptRunner::run));

	emit invoker.invoke();
}