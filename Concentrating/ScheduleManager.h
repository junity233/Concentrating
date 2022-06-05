#pragma once
#include <qvector.h>
#include <qdatetime.h>
#include <qobject.h>

#include "CronTime.h"

class LuaScriptRunner;
class QUrl;

class ScheduleManager:
    public QObject
{
    Q_OBJECT
    ScheduleManager();
public:
    static ScheduleManager* instance() { return _instance; }

    struct Task {
        CronTime time;
        QString script;
        bool enable = false;
    };

    void addTask(const CronTime& time, const QString& script,bool enable);
    void removeTask(int idx);

    QVector<Task> const& tasks()const { return _tasks; }
    void setTasks(const QVector<Task>& tasks);

    void load();
    void store();
public slots:
    void schedule();

signals:
    void runScript(const QUrl& path);

private:
    static ScheduleManager* _instance;

    QVector<Task> _tasks;
};

