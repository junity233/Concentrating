#include "SettingPage.h"
#include "SettingTableModel.h"
#include "SettingManager.h"
#include "ScriptManager.h"
#include "AutoStartHelper.h"
#include "QVariantModel.h"
#include "ScheduleDelegate.h"
#include "QVariantDelegate.h"
#include "ScheduleTableModel.h"

#include <qmenu.h>

SettingPage::SettingPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &SettingPage::reset);

	settingModel = new QVariantModel(this);
	settingDelegate = new QVariantDelegate(this);
	ui.scriptSettingView->setModel(settingModel);
	ui.scriptSettingView->setItemDelegateForColumn(2, settingDelegate);

	scheduleDelegate = new ScheduleDelegate(this);
	ui.scheduleView->setItemDelegate(scheduleDelegate);

	scheduleModel = new ScheduleTableModel(this);
	ui.scheduleView->setModel(scheduleModel);

	connect(ui.newTask, &QPushButton::clicked, [this]() {
		scheduleModel->insertRow(scheduleModel->rowCount(QModelIndex()) - 1, QModelIndex());
		});

	connect(ui.deleteTask, &QPushButton::clicked, [this]() {
		scheduleModel->removeRow(ui.scheduleView->currentIndex().row(), QModelIndex());
		});

	setupMenu();

	reset();
}

SettingPage::~SettingPage()
{
}

void SettingPage::submit(int idx)
{
	if (idx == -1)
		idx = ui.tabWidget->currentIndex();

	switch (idx) {
	case 0:
		submitGeneral();
		break;
	case 1:
		submitScript();
		break;
	case 2:
		submitBrowser();
		break;
	case 3:
		submitSchedule();
	}
}

void SettingPage::newSetting()
{
	QModelIndex index = ui.scriptSettingView->currentIndex();

	settingModel->insertRow(0, index);
	ui.scriptSettingView->edit(index.child(settingModel->rowCount(index) - 1, 0));
}

void SettingPage::deleteSetting()
{
	auto index = ui.scriptSettingView->currentIndex();
	if (!index.isValid())
		return;

	int row = index.row();
	settingModel->removeRow(row, index);
}

void SettingPage::resetGeneral()
{
	auto instance = SettingManager::instance();

	int cnt = ScriptManager::instance()->scriptCount();
	ui.autoStartScript->clear();

	ui.autoStartScript->addItem("(No script)");
	for (int i = 0; i < cnt; i++) {
		ui.autoStartScript->addItem(ScriptManager::instance()->script(i).name);
	}
	int idx = instance->value("system.autostart.script", "").toInt();
	if (idx >= 0 && idx < ScriptManager::instance()->scriptCount())
		ui.autoStartScript->setCurrentText(ScriptManager::instance()->script(idx).name);
	else ui.autoStartScript->setCurrentIndex(-1);

	ui.noticeScriptStart->setChecked(instance->value("system.notice_script_start", true).toBool());
	ui.noticeScriptFinish->setChecked(instance->value("system.notice_script_finished", true).toBool());
	ui.noticeScriptFailed->setChecked(instance->value("system.notice_script_failed", true).toBool());
	ui.enablePassword->setChecked(instance->value("system.enable_password", false).toBool());
	ui.password->setText(instance->value("system.password", QString()).toString());
}

void SettingPage::resetScript()
{
	settingModel->setVariant(SettingManager::instance()->data());
}

void SettingPage::resetBrowser()
{
	auto instance = SettingManager::instance();
	ui.browserDefaultUrl->setText(instance->value("system.browser.default_page_url").toString());
}

void SettingPage::resetSchedule()
{
	scheduleModel->reset();
}

void SettingPage::submitGeneral()
{
	auto instance = SettingManager::instance();

	instance->setValue("system.autostart.script", ui.autoStartScript->currentIndex() - 1);
	instance->setValue("system.notice_script_start", ui.noticeScriptStart->isChecked());
	instance->setValue("system.notice_script_finished", ui.noticeScriptFinish->isChecked());
	instance->setValue("system.notice_script_failed", ui.noticeScriptFailed->isChecked());
	instance->setValue("system.enable_password", ui.enablePassword->isChecked());
	instance->setValue("system.password", ui.password->text());

	AutoStartHelper::setAutoStartEnable(ui.enableAutoStart->isChecked());
}

void SettingPage::submitScript()
{
	SettingManager::instance()->setData(settingModel->variant().toMap());
}

void SettingPage::submitBrowser()
{
	auto instance = SettingManager::instance();

	instance->setValue("system.browser.default_page_url", ui.browserDefaultUrl->text());

}

void SettingPage::submitSchedule()
{
	scheduleModel->store();
}

void SettingPage::enableAutoStart()
{
	AutoStartHelper::setAutoStartEnable(true);
}

void SettingPage::disableAutoStart()
{
	AutoStartHelper::setAutoStartEnable(false);
}

void SettingPage::setupMenu()
{
	QMenu* scriptMenu = new QMenu(this);

	QAction* newItemAction = new QAction(tr("New Item"), this);
	QAction* deleteItemAction = new QAction(tr("Delete Item"), this);

	scriptMenu->addActions({ newItemAction,deleteItemAction });

	ui.scriptSettingView->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(ui.scriptSettingView, &QTreeView::customContextMenuRequested, [this, scriptMenu](const QPoint& pos) {
		scriptMenu->exec(QCursor::pos());
		});

	connect(newItemAction, &QAction::triggered, this, &SettingPage::newSetting);
	connect(deleteItemAction, &QAction::triggered, this, &SettingPage::deleteSetting);
}

void SettingPage::reset(int idx)
{
	if (idx == -1)
		idx = ui.tabWidget->currentIndex();

	switch (idx) {
	case 0:
		resetGeneral();
		break;
	case 1:
		resetScript();
		break;
	case 2:
		resetBrowser();
		break;
	case 3:
		resetSchedule();
	}
}

