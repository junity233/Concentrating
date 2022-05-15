#include "MainWindow.h"
#include <qevent.h>
#include <qsystemtrayicon.h>
#include <ConcerntratingBrowser.h>
#include <qdesktopwidget.h>
#include <qmessagebox.h>
#include <qmenu.h>
#include <qaction.h>
#include <qcursor.h>
#include <qdebug.h>
#include <qresource.h>
#include <qtimer.h>

#include "SettingManager.h"
#include "ScriptManager.h"
#include "ScheduleManager.h"
#include "LuaBinding.h"
#include "ProcessProtecter.h"
#include "ProcessHelper.h"

MainWindow* MainWindow::_instance = Q_NULLPTR;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    SettingManager::instance()->load("settings.json");
    ScriptManager::instance()->load("scripts.json");

    setWindowIcon(QIcon(":/icon.ico"));

    setupBrowser();

    _instance = this;

    ui.setupUi(this);

    setupScriptPage();
    setupLuaBinder();
    
    connect(ui.tabWidget, &QTabWidget::currentChanged, [this](int idx) {
        if (idx == 1)
            ui.settingPage->reset(-1);
        });

    connect(ScheduleManager::instance(), &ScheduleManager::runScript, ui.scriptPage, &ScriptPage::runLuaScript);

    setupSystemTray();

    _timer = new QTimer(this);
    _timer->setInterval(1000);
    _timer->start();
    connect(_timer, &QTimer::timeout, ScheduleManager::instance(), &ScheduleManager::schedule);

    emit initializeFinished();
}

void MainWindow::scriptRunFailed(const QString& reason) {
    ui.logPage->log(reason,LogPage::Lua);

    if (SettingManager::instance()->value("system.notice_script_failed", false).toBool())
        _systemTray->showMessage(tr("Notice"), tr("Script runs failed:%1").arg(reason));
}

void MainWindow::scriptRunFinished(bool exitCode)
{
    QString msg = tr("Script runs completely,exit code:%1").arg(exitCode);
    ui.logPage->log(msg, LogPage::System);
    statusBar()->showMessage(msg, 3);

    if (exitCode == 0 && SettingManager::instance()->value("system.notice_script_finished", false).toBool())
        _systemTray->showMessage(tr("Notice"), tr("Script runs finished"));
}

void MainWindow::scriptAboutToRun(int idx)
{
    QString name = ScriptManager::instance()->script(idx).name;

    ui.logPage->log(tr("Script begin to run:%1").arg(name), LogPage::System);

    if (SettingManager::instance()->value("system.notice_script_start", false).toBool())
        _systemTray->showMessage(tr("Notice"), tr("Script \"%1\" is running!").arg(name));
}

void MainWindow::systemTrayActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
    case QSystemTrayIcon::Trigger:
    case QSystemTrayIcon::DoubleClick:
        this->showWindow();
        break;
    case QSystemTrayIcon::Context:
        _menu->exec(QCursor::pos());
        break;
    }
}

void MainWindow::closeEvent(QCloseEvent* event)
{
    ScriptManager::instance()->save("scripts.json");
    SettingManager::instance()->save("settings.json");

    event->ignore();
    this->hide();
}

void MainWindow::setupSystemTray()
{
    _systemTray = new QSystemTrayIcon(this);

    _systemTray->setToolTip(tr("Concerntrating"));

    _menu = new QMenu(this);

    auto showAction = new QAction(tr("Show"), this);
    auto exitAction = new QAction(tr("Exit"), this);

    connect(showAction, &QAction::triggered, [this]() {
        this->showWindow();
        });

    connect(exitAction, &QAction::triggered, [this]() {
        if (!ui.scriptPage->isScriptRunning())
        {
            if (ProcessProtecter::isProtected())
                ProcessProtecter::unprotect();

            QApplication::exit(0);
        }
        else {
            QMessageBox::warning(nullptr, tr("Note"), tr("A script is still running!Exiting is banned!"));
        }
        });

    _menu->addAction(showAction);
    _menu->addAction(exitAction);
    

    _systemTray->setContextMenu(_menu);
    _systemTray->setIcon(QIcon(":/icon.ico"));

    connect(_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::systemTrayActived);

    _systemTray->show();
}

void MainWindow::showWindow()
{
    if (this->isHidden())
        this->show();
}

void MainWindow::setupBrowser()
{
    _browser = new ConcerntratingBrowser(this);
    QRect rect = QApplication::desktop()->screenGeometry();
    _browser->setGeometry(0, 0, rect.width(), rect.height());

    _browser->hide();
}

void MainWindow::setupScriptPage()
{
    connect(ui.scriptPage, &ScriptPage::scriptRunFailed, this, &MainWindow::scriptRunFailed);
    connect(ui.scriptPage, &ScriptPage::statusBarMessage, [this](const QString& msg, int timeout) {
        statusBar()->showMessage(msg, timeout);
        });
    connect(ui.scriptPage, &ScriptPage::scriptAboutToRun, this, &MainWindow::scriptAboutToRun);

    connect(ui.scriptPage, &ScriptPage::scriptRunFinished, this, &MainWindow::scriptRunFinished);

}

void MainWindow::setupLuaBinder()
{
    LuaBinder* binder = LuaBinder::instance();

    connect(binder, &LuaBinder::openBrowser, _browser, &ConcerntratingBrowser::showFullScreen);
    connect(binder, &LuaBinder::closeBrowser, _browser, &ConcerntratingBrowser::hide);
    connect(binder, &LuaBinder::browserLoadUrl, _browser, &ConcerntratingBrowser::load);
    LuaBinder::instance()->setLogCallback([this](const QString& msg) {
        ui.logPage->log(msg, LogPage::Script);
        });
}
