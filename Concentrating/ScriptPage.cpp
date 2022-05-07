#include "ScriptPage.h"
#include "ScriptManager.h"
#include "ScriptListModel.h"
#include "MainWindow.h"
#include "LuaScriptRunner.h"
#include "ProcessHelper.h"
#include "ProcessProtecter.h"

#include <QLuaCompleter.hpp>
#include <QLuaHighlighter.hpp>
#include <qthread.h>
#include <qmessagebox.h>

#include <lua.hpp>

ScriptPage::ScriptPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	model = new ScriptListModel(this);
	_thread = new QThread(this);
	runner = new LuaScriptRunner;

	runner->moveToThread(_thread);

	_thread->start();

	auto completer = new QLuaCompleter(ui.codeEditor);
	auto highlighter = new QLuaHighlighter(ui.codeEditor->document());

	ui.listView->setModel(model);
	ui.codeEditor->setCompleter(completer);
	ui.codeEditor->setHighlighter(highlighter);

	connect(runner, &LuaScriptRunner::scriptRunFailed, this, &ScriptPage::scriptRunFailed);
	connect(runner, &LuaScriptRunner::scriptRunFinished, [this](int exitCode) {
		ProcessProtecter::unprotect();
		emit scriptRunFinished(exitCode);
		});
	connect(this, &ScriptPage::runLuaScript, runner, &LuaScriptRunner::run);

	currentIndex = -1;
	updateScript(0);
}

ScriptPage::~ScriptPage()
{
	delete runner;

	_thread->quit();
	_thread->wait();
}

bool ScriptPage::isScriptRunning() const
{
	return runner->isRunning();
}

void ScriptPage::newScript()
{
	auto index=model->append();
	ui.listView->edit(index);
	updateScript(index.row());
}

void ScriptPage::deleteScript()
{
	auto index = ui.listView->currentIndex();
	model->remove(index);
	updateScript(0);
}

void ScriptPage::listViewClicked(const QModelIndex& index)
{
	updateScript(index.row());
}

void ScriptPage::runScript()
{
	saveCurrentScript();
	if (runner->isRunning())
		QMessageBox::information(this, tr("Note"), tr("A Script is already running"));
	else {
		ProcessProtecter::protect(ProcessHelper::currentPid());
		emit runLuaScript(currentIndex);
	}
}


void ScriptPage::updateScript(int index)
{
	if (index >= ScriptManager::instance()->scriptCount() || index < 0) {
		ui.codeEditor->setEnabled(false);
		return;
	}

	ui.codeEditor->setEnabled(true);

	saveCurrentScript();

	QString code = ScriptManager::instance()->script(index).code;
	ui.codeEditor->setText(code);

	currentIndex = index;
}

void ScriptPage::saveCurrentScript()
{
	if (currentIndex != -1)
		ScriptManager::instance()->script(currentIndex).code = ui.codeEditor->toPlainText();
}

void ScriptPage::closeEvent(QCloseEvent* event)
{
	saveCurrentScript();
	if (runner->isRunning())
		event->ignore();
}
