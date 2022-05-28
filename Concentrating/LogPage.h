#pragma once

#include <QWidget>
#include <qmutex.h>
#include "ui_LogPage.h"

class LogPage : public QWidget
{
	Q_OBJECT

public:
	LogPage(QWidget *parent = Q_NULLPTR);
	~LogPage();

	enum Role
	{
		Script,
		System,
		Lua
	};
public slots:
	void log(const QString& msg, int role);

private:
	Ui::LogPage ui;
};
