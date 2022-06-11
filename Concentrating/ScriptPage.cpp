#include "ScriptPage.h"

#include <QLuaCompleter.hpp>
#include <QLuaHighlighter.hpp>
#include <qmessagebox.h>
#include <qfile.h>
#include <qfiledialog.h>
#include <qevent.h>
#include <qdir.h>

#include <lua.hpp>

ScriptPage::ScriptPage(QWidget *parent)
	: QWidget(parent),
	_path(),
	_savedCode()
{
	ui.setupUi(this);

	auto completer = new QLuaCompleter(ui.codeEditor);
	auto highlighter = new QLuaHighlighter(ui.codeEditor->document());

	ui.codeEditor->setCompleter(completer);
	ui.codeEditor->setHighlighter(highlighter);

	connect(ui.runScrupt, &QPushButton::clicked, this, [this]() {
		if (modified())
			save();
		});
	connect(ui.runScrupt, &QPushButton::clicked, this, &ScriptPage::run);
	
	connect(ui.codeEditor, &QCodeEditor::textChanged, this, [this]() {
		emit modifyChanged(code() == this->_savedCode);
		});
}

ScriptPage::~ScriptPage()
{

}

QString ScriptPage::code() const
{
	return ui.codeEditor->toPlainText();
}

void ScriptPage::save()
{
	if (_path.isEmpty()) {
		_path = getPath();

		if (_path.isEmpty())
			return;

		emit pathChanged(_path);
	}

	QFile f(_path.toLocalFile());
	f.open(QIODevice::WriteOnly);

	if (!f.isOpen()) {
		QMessageBox::critical(
			this,
			tr("Error"),
			tr("Open file %1 failed!").arg(_path.toLocalFile())
		);

		return;
	}

	f.write(code().toUtf8());
	_savedCode = code();

	emit modifyChanged(false);

	f.close();
}

void ScriptPage::setPath(const QUrl& path)
{
	_path = path;
	emit pathChanged(_path);
}

void ScriptPage::saveAs()
{
	QUrl newPath = getPath();

	if (newPath.isEmpty())
		return;

	_path = newPath;
	emit pathChanged(_path);
	save();
}

QUrl ScriptPage::getPath()
{
	return QFileDialog::getSaveFileUrl(
		this,
		tr("Save script..."),
		QUrl::fromLocalFile("scripts"),
		tr("Lua scripts (*.lua)")
	);
}
