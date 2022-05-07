#include "MainWindow.h"
#include "SettingManager.h"
#include "ScriptManager.h"
#include <qevent.h>
#include <ConcerntratingBrowser.h>
#include <qdesktopwidget.h>
#include "LuaBinding.h"

#include "ProcessProtecter.h"
#include "ProcessHelper.h"

MainWindow* MainWindow::_instance = Q_NULLPTR;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    SettingManager::instance()->load("settings.json");
    ScriptManager::instance()->load("scripts.json");


    ui.setupUi(this);

    _browser = new ConcerntratingBrowser(this);
    QRect rect = QApplication::desktop()->screenGeometry();
    _browser->setGeometry(0, 0, rect.width(), rect.height());

    _browser->hide();

    LuaBinder::instance()->setLogCallback([this](const QString& msg) {
        ui.logPage->log(msg, LogPage::Script);
        });

    connect(ui.scriptPage, &ScriptPage::scriptRunFailed, this, &MainWindow::scriptRunFailed);
    connect(ui.scriptPage, &ScriptPage::statusBarMessage, [this](const QString& msg, int timeout) {
        statusBar()->showMessage(msg, timeout);
        });

    connect(ui.scriptPage, &ScriptPage::scriptRunFinished, this, &MainWindow::scriptRunFinished);



    connect(LuaBinder::instance(), &LuaBinder::openBrowser,_browser,&ConcerntratingBrowser::showFullScreen);
    connect(LuaBinder::instance(), &LuaBinder::closeBrowser, _browser, &ConcerntratingBrowser::hide);

    connect(ui.tabWidget, &QTabWidget::currentChanged, [this](int idx) {
        if (idx == 1)
            ui.settingPage->reset(-1);
        });
    _instance = this;
}

void MainWindow::scriptRunFailed(const QString& reason) {
    ui.logPage->log(tr("Script runs failed:\n\t%1").arg(reason),LogPage::Lua);
}

void MainWindow::scriptRunFinished(int exitCode)
{
    QString msg = tr("Script runs completely,exit code:%1").arg(exitCode);
    ui.logPage->log(msg, LogPage::System);
    statusBar()->showMessage(msg);
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    ScriptManager::instance()->save("scripts.json");
    SettingManager::instance()->save("settings.json");

    if (ui.scriptPage->isScriptRunning())
        event->ignore();
    else
        ProcessProtecter::unprotect();
}