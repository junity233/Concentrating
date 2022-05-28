#pragma once

#include <QWidget>
#include "ui_ScriptPage.h"

class ScriptManager;
class ScriptListModel;
class QMainWindow;
class QThread;
class LuaScriptRunnerPool;
class QThreadPool;

struct lua_State;

class ScriptPage : public QWidget
{
	Q_OBJECT

public:
	ScriptPage(QWidget *parent = Q_NULLPTR);
	~ScriptPage();

	bool isScriptRunning()const;

signals:
	void scriptRunFailed(const QString& reason);
	void scriptRunFinished(bool exitCode);
	void scriptAboutToRun(int idx);

public slots:
	void runScript(int index = -1);
	void save();

private slots:
	void newScript();
	void deleteScript();

	void listViewClicked(const QModelIndex& index);
	

private:
	void updateScript(int index);
	void saveScript(int idx);
	void runAutoStartScript();

	int currentScript()const;

protected:
	void closeEvent(QCloseEvent* event);

private:
	Ui::ScriptPage ui;

	ScriptListModel* model;
	int lastIndex;

	LuaScriptRunnerPool* runnerPool;
};
