#pragma once

#include <QWidget>
#include "ui_ScriptPage.h"

class ScriptManager;
class ScriptListModel;
class QMainWindow;
class QThread;
class LuaScriptRunner;

struct lua_State;

class ScriptPage : public QWidget
{
	Q_OBJECT

public:
	ScriptPage(QWidget *parent = Q_NULLPTR);
	~ScriptPage();

	bool isScriptRunning()const;

signals:
	void statusBarMessage(const QString& msg, int timeout = 0);
	void scriptRunFailed(const QString& reason);
	void scriptRunFinished(int exitCode);

	void runLuaScript(int index);

private slots:
	void newScript();
	void deleteScript();

	void listViewClicked(const QModelIndex& index);

	void runScript();
	

private:
	void updateScript(int index);
	void saveCurrentScript();

protected:
	void closeEvent(QCloseEvent* event);

private:
	Ui::ScriptPage ui;

	ScriptListModel* model;
	int currentIndex;

	QThread* _thread;
	LuaScriptRunner* runner;
};
