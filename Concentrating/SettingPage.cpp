#include "SettingPage.h"
#include "SettingTableModel.h"
#include "SettingManager.h"
#include "ScriptManager.h"
#include "AutoStartHelper.h"

SettingPage::SettingPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);


	connect(ui.tabWidget, &QTabWidget::currentChanged, this, &SettingPage::reset);

	model = new SettingTableModel(this);
	ui.scriptSettingView->setModel(model);

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
	}
}

void SettingPage::newSetting()
{
	model->newSetting();
}

void SettingPage::deleteSetting()
{
	auto index = ui.scriptSettingView->currentIndex();
	if (!index.isValid())
		return;

	int row = index.row();
	model->deleteSetting(row);
}

void SettingPage::resetGeneral()
{
	auto instance = SettingManager::instance();

	int cnt = ScriptManager::instance()->scriptCount();
	ui.autoStartScript->clear();

	for (int i = 0; i < cnt; i++) {
		ui.autoStartScript->addItem(ScriptManager::instance()->script(i).name);
	}
	int idx = instance->value("autostart.script", "").toInt();
	ui.autoStartScript->setCurrentText(ScriptManager::instance()->script(idx).name);

	ui.noticeScriptStart->setChecked(instance->value("system.notice_script_start", true).toBool());
	ui.noticeScriptFinish->setChecked(instance->value("system.notice_script_finished", true).toBool());
	ui.noticeScriptFailed->setChecked(instance->value("system.notice_script_failed", true).toBool());
}

void SettingPage::resetScript()
{
	model->reset();
}

void SettingPage::resetBrowser()
{
	auto instance = SettingManager::instance();
	ui.browserDefaultUrl->setText(instance->value("browser.default_page_url").toString());
}

void SettingPage::submitGeneral()
{
	auto instance = SettingManager::instance();
	instance->setValue("autostart.script", ui.autoStartScript->currentIndex());
	instance->setValue("system.notice_script_start", ui.noticeScriptStart->isChecked());
	instance->setValue("system.notice_script_finished", ui.noticeScriptFinish->isChecked());
	instance->setValue("system.notice_script_failed", ui.noticeScriptFailed->isChecked());

	AutoStartHelper::setAutoStartEnable(ui.enableAutoStart->isChecked());
}

void SettingPage::submitScript()
{
	model->store();
}

void SettingPage::submitBrowser()
{
	auto instance = SettingManager::instance();

	instance->setValue("browser.default_page_url", ui.browserDefaultUrl->text());

}

void SettingPage::enableAutoStart()
{
	AutoStartHelper::setAutoStartEnable(true);
}

void SettingPage::disableAutoStart()
{
	AutoStartHelper::setAutoStartEnable(false);
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
	}
}

