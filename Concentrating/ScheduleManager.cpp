#include "ScheduleManager.h"
#include "SettingManager.h"

#include <qurl.h>
#include <qdebug.h>

ScheduleManager* ScheduleManager::_instance = new ScheduleManager();

ScheduleManager::ScheduleManager():
	QObject(Q_NULLPTR)
{

}

void ScheduleManager::addTask(const CronTime& time, const QString& script,bool enable)
{
	_tasks.append({ time,script ,enable});
}

void ScheduleManager::removeTask(int idx)
{
	_tasks.remove(idx);
}

void ScheduleManager::setTasks(const QVector<Task>& tasks)
{
	_tasks = tasks;
}

void ScheduleManager::load()
{
	SettingManager* instance = SettingManager::instance();

	QVariantList data = instance->value("system.schedule", QVariantList()).toList();
	_tasks.clear();

	for (auto i : data) {
		QVariantMap task = i.toMap();

		addTask(CronTime::fromString(task["time"].toString()), task["script"].toString(), task["enable"].toBool());
	}
}

void ScheduleManager::store()
{
	SettingManager* instance = SettingManager::instance();

	QVariantList data;

	for (auto i:_tasks) {
		QVariantMap task;
		task["time"] = i.time.toString();
		task["script"] = i.script;
		task["enable"] = i.enable;

		data.append(task);
	}

	instance->setValue("system.schedule", data);
}

void ScheduleManager::schedule() {
	QDateTime time = QDateTime::currentDateTime();

	for (auto i : _tasks) {
		if (i.enable && i.time.check(time)) {
			emit runScript(QUrl::fromLocalFile(i.script));
			return;
		}
	}
}
