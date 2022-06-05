#pragma once

#include <QWidget>
#include <qurl.h>
#include "ui_ScriptPage.h"

class QCloseEvent;

class ScriptPage : public QWidget
{
	Q_OBJECT

public:
	ScriptPage(QWidget *parent = Q_NULLPTR);
	~ScriptPage();

	QString code()const;
	inline void setCode(const QString& code) { ui.codeEditor->setText(code); _savedCode = code; emit modifyChanged(false); }

	inline QUrl path()const {
		return _path;
	}
	;
	void setPath(const QUrl& path);

	bool modified()const { return code() != _savedCode; }

signals:
	void run();
	void modifyChanged(bool modified);
	void pathChanged(const QUrl& path);

public slots:
	void save();
	void saveAs();

private:
	QUrl getPath();

private:
	Ui::ScriptPage ui;

	QUrl _path;
	QString _savedCode;
};
