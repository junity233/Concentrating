#pragma once

#include <QWidget>
#include <qcheckbox.h>
#include "ui_SettingPage.h"

class SettingTableModel;

class SettingPage : public QWidget
{
	Q_OBJECT

public:
	SettingPage(QWidget *parent = Q_NULLPTR);
	~SettingPage();

public slots:
	void reset(int idx = -1);
	void submit(int idx = -1);
	void newSetting();
	void deleteSetting();

	void resetGeneral();
	void resetScript();
	void resetBrowser();

	void submitGeneral();
	void submitScript();
	void submitBrowser();

private slots:
	void enableAutoStart();
	void disableAutoStart();

private:
	Ui::SettingPage ui;
	SettingTableModel* model;
};
