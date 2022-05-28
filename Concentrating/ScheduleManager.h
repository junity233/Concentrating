#pragma once
#include <qvector.h>
#include <qdatetime.h>
#include <qobject.h>

#include "CronTime.h"

class LuaScriptRunner;

class ScheduleManager:
    public QObject
{
    Q_OBJECT
    ScheduleManager();
public:
    static ScheduleManager* instance() { return _instance; }

    struct Task {
        CronTime time;
        int script;
        bool enable = false;
    };

    void addTask(const CronTime& time, int script,bool enable);
    void removeTask(int idx);

    QVector<Task> const& tasks()const { return _tasks; }
    void setTasks(const QVector<Task>& tasks);

    void load();
    void store();
public slots:
    void schedule();

signals:
    void runScript(int script);

private:
    static ScheduleManager* _instance;

    QVector<Task> _tasks;
};

