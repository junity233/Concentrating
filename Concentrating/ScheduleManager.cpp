#include "ScheduleManager.h"
#include "SettingManager.h"

ScheduleManager* ScheduleManager::_instance = new ScheduleManager();

ScheduleManager::ScheduleManager():
	QObject(Q_NULLPTR)
{

}

void ScheduleManager::addTask(QTime time, int script)
{
	_tasks.append({ time,script });
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

		addTask(task["time"].toTime(), task["script"].toInt());
	}
}

void ScheduleManager::store()
{
	SettingManager* instance = SettingManager::instance();

	QVariantList data;

	for (auto i:_tasks) {
		QVariantMap task;
		task["time"] = i.time;
		task["script"] = i.script;

		data.append(task);
	}

	instance->setValue("system.schedule", data);
}

void ScheduleManager::schedule() {
	QTime time = QTime::currentTime();
	time.setHMS(time.hour(), time.minute(), time.second());

	for (auto i : _tasks) {
		if (i.time == time) {
			emit runScript(i.script);
			return;
		}
	}
}
