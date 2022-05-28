#include "ScriptPage.h"
#include "ScriptManager.h"
#include "ScriptListModel.h"
#include "MainWindow.h"
#include "LuaScriptRunner.h"
#include "ProcessHelper.h"
#include "ProcessProtecter.h"
#include "SettingManager.h"
#include "AutoStartHelper.h"
#include "LuaScriptRunnerPool.h"
#include "LogPage.h"

#include <QLuaCompleter.hpp>
#include <QLuaHighlighter.hpp>
#include <qthread.h>
#include <qmessagebox.h>
#include <qthreadpool.h>

#include <lua.hpp>

ScriptPage::ScriptPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	model = new ScriptListModel(this);
	runnerPool = new LuaScriptRunnerPool(this);

	auto completer = new QLuaCompleter(ui.codeEditor);
	auto highlighter = new QLuaHighlighter(ui.codeEditor->document());

	ui.listView->setModel(model);
	ui.codeEditor->setCompleter(completer);
	ui.codeEditor->setHighlighter(highlighter);

	connect(MainWindow::instance(), &MainWindow::initializeFinished, [this]() {
		if (AutoStartHelper::isApplicationAutoStart())
			runAutoStartScript();
		});

	connect(runnerPool, &LuaScriptRunnerPool::finished, this, [this](const QString& name, bool exitCode) {
		QString msg;

		if (!exitCode)
			msg = tr("Script \"%1\" run completely.").arg(name);
		else
			msg = tr("Script \"%1\" run failed.").arg(name);

		MainWindow::instance()->log(msg, LogPage::System);
		MainWindow::instance()->statusBarMessage(msg);

		if (SettingManager::instance()->value("system.notice_script_finished", true).toBool())
			MainWindow::instance()->systemTray()->showMessage(tr("Notice"), msg);

		});

	connect(runnerPool, &LuaScriptRunnerPool::failed, this, [this](const QString& name, const QString& reason) {
		MainWindow* mainWindow = MainWindow::instance();
		mainWindow->log(reason, LogPage::Lua);

		if (SettingManager::instance()->value("system.notice_script_failed", true).toBool())
			mainWindow->systemTray()->showMessage(tr("Notice"), tr("Script \"%1\" runs failed!").arg(name));
		});

	connect(ui.runScrupt, &QPushButton::clicked, this, [this]() {
		runScript();
		});

	lastIndex = -1;
	updateScript(0);

}

ScriptPage::~ScriptPage()
{

}

bool ScriptPage::isScriptRunning() const
{
	return !runnerPool->isFinished();
}

void ScriptPage::newScript()
{
	save();
	auto index=model->append();
	ui.listView->edit(index);
	updateScript(index.row());
}

void ScriptPage::deleteScript()
{
	save();
	auto index = ui.listView->currentIndex();
	model->remove(index);
	updateScript(0);
}

void ScriptPage::listViewClicked(const QModelIndex& index)
{
	save();
	updateScript(index.row());
}

void ScriptPage::runScript(int index)
{
	save();

	if (index == -1)
		index = currentScript();

	if (index == -1)
		return;

	ScriptManager::Script script = ScriptManager::instance()->script(index);
	QString msg = tr(R"(Script "%1" start to run!)").arg(script.name);

	if (SettingManager::instance()->value("system.notice_script_start", true).toBool()) {
		MainWindow::instance()->systemTray()->showMessage(tr("Notice"), msg);
	}

	MainWindow::instance()->log(msg, LogPage::System);
	MainWindow::instance()->statusBar()->showMessage(msg);

	runnerPool->run(script.name, script.code);
}


void ScriptPage::updateScript(int index)
{
	if (index >= ScriptManager::instance()->scriptCount() || index < 0) {
		ui.codeEditor->setEnabled(false);
		ui.codeEditor->setText("");
		return;
	}

	ui.codeEditor->setEnabled(true);

	lastIndex = index;

	QString code = ScriptManager::instance()->script(index).code;
	ui.codeEditor->setText(code);

	ui.listView->setCurrentIndex(model->index(index));
}

void ScriptPage::saveScript(int idx)
{
	if (idx != -1)
		ScriptManager::instance()->script(idx).code = ui.codeEditor->toPlainText();
}

void ScriptPage::runAutoStartScript()
{
	int idx = SettingManager::instance()->value("autostart.script", 0).toInt();
	emit scriptAboutToRun(idx);
	emit runScript(idx);
}

int ScriptPage::currentScript() const
{
	auto idx = ui.listView->currentIndex();
	if (idx.isValid())
		return idx.row();
	return -1;
}

void ScriptPage::closeEvent(QCloseEvent* event)
{
	save();
	if (!runnerPool->isFinished())
		event->ignore();
}

void ScriptPage::save() {
	saveScript(lastIndex);
}
