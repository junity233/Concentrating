#pragma once

#include <QWidget>
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

	void log(const QString& msg, Role role);

private:
	Ui::LogPage ui;
};
