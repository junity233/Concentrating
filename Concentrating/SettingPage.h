#pragma once

#include <QWidget>
#include "ui_SettingPage.h"

class SettingTableModel;
class QVariantModel;
class ScheduleDelegate;
class ScheduleTableModel;
class QVariantDelegate;

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
	void resetSchedule();

	void submitGeneral();
	void submitScript();
	void submitBrowser();
	void submitSchedule();

private slots:
	void enableAutoStart();
	void disableAutoStart();

private:
	Ui::SettingPage ui;
	QVariantModel* settingModel;
	QVariantDelegate* settingDelegate;
	ScheduleTableModel* scheduleModel;
	ScheduleDelegate* scheduleDelegate;
};
