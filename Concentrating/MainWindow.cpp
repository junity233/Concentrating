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
#include <qinputdialog.h>

#include "MusicPlayer.h"
#include "MethodInvoker.hpp"
#include "SettingManager.h"
#include "ScriptManager.h"
#include "ScheduleManager.h"
#include "LuaBinding.h"
#include "ProcessProtecter.h"
#include "ProcessHelper.h"
#include "MouseHelper.h"
#include "KeyboardHelper.h"
#include "TextSpeaker.h"

#include "version.h"

MainWindow* MainWindow::_instance = Q_NULLPTR;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    SettingManager::instance()->load("config.dat");
    ScriptManager::instance()->load("scripts.json");

    setupBrowser();

    _instance = this;

    ui.setupUi(this);


    this->log("Concentrating(" COMPILE_DATETIME ") [" COMPILER "] on " SYSTEM, LogPage::System);
    this->log(tr("Initialization begin."), LogPage::System);

    this->log(tr("Setup script system."), LogPage::System);

    setupLuaBinder();

    connect(ui.tabWidget, &QTabWidget::currentChanged, [this](int idx) {
        if (idx == 1)
            ui.settingPage->reset(-1);
        });

    connect(ScheduleManager::instance(), &ScheduleManager::runScript, ui.scriptPage, &ScriptPage::runScript);

    this->log(tr("Setup system tray."), LogPage::System);
    setupSystemTray();


    this->log(tr("Setup schedule."), LogPage::System);
    _timer = new QTimer(this);
    _timer->setInterval(1000);
    _timer->start();
    connect(_timer, &QTimer::timeout, ScheduleManager::instance(), &ScheduleManager::schedule);


    this->log(tr("Setup process protection."), LogPage::System);
    ProcessProtecter::protect(ProcessHelper::currentPid());

    MusicPlayer::setPath(QDir::current().absoluteFilePath("music"));

    emit initializeFinished();
}
void MainWindow::log(const QString& msg, LogPage::Role role) {
    mutex.lock();
    ui.logPage->log(msg, role);
    mutex.unlock();
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
    save();
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


    connect(showAction, &QAction::triggered, this, &MainWindow::showWindow);
    connect(exitAction, &QAction::triggered, this, &MainWindow::exit);
        
    _menu->addAction(showAction);
    _menu->addAction(exitAction);
    

    _systemTray->setContextMenu(_menu);
    _systemTray->setIcon(QIcon(":/icon.ico"));

    connect(_systemTray, &QSystemTrayIcon::activated, this, &MainWindow::systemTrayActived);

    _systemTray->show();
}

void MainWindow::showWindow()
{
    if (!checkPassword())
        this->hide();
    else
        this->show();

    this->log(tr("Window shows."),LogPage::System);
}

void MainWindow::setupBrowser()
{
    _browser = new ConcerntratingBrowser(this);
    QRect rect = QApplication::desktop()->screenGeometry();
    _browser->setGeometry(0, 0, rect.width(), rect.height());

    _browser->hide();
}

void MainWindow::setupLuaBinder()
{
    LuaBinder* binder = LuaBinder::instance();

    connect(binder, &LuaBinder::openBrowser, _browser, &ConcerntratingBrowser::showFullScreen);
    connect(binder, &LuaBinder::closeBrowser, _browser, &ConcerntratingBrowser::hide);
    connect(binder, &LuaBinder::browserLoadUrl, _browser, &ConcerntratingBrowser::load);
    connect(binder, &LuaBinder::log, this, [this](const QString& msg) {
        this->log(msg, LogPage::Script);
        });

    connect(binder, &LuaBinder::lockKeyboard, this, [this]() {
        this->log(tr("Keyboard locked."), LogPage::System);
        KeyboardHelper::lock();
        });

    connect(binder, &LuaBinder::unlockKeyboard, this, [this]() {
        this->log(tr("Keybiard unlocked."), LogPage::System);
        KeyboardHelper::unlock();
        });

    connect(binder, &LuaBinder::lockMouse, this, [this]() {
        this->log(tr("Mouse locked."), LogPage::System);
        MouseHelper::lock();
        });

    connect(binder, &LuaBinder::unlockMouse, this, [this]() {
        this->log(tr("Mouse unlocked."), LogPage::System);
        MouseHelper::unlock();
        });

    connect(binder, &LuaBinder::say, this, [](const QString& text) {TextSpeaker::say(text); });

    TextSpeaker::load();
}

bool MainWindow::checkPassword()
{
    if (SettingManager::instance()->value("system.enable_password", false).toBool() == false)
        return true;
    bool ok = true;
    QString password = QInputDialog::getText(this, tr("Password"), tr("Please input the password to continue:"), QLineEdit::Password, "", &ok, Qt::MSWindowsFixedSizeDialogHint);
    if (!ok || password != SettingManager::instance()->value("system.password", "").toString()) {
        if(ok)
            QMessageBox::information(this,tr("Notice"), tr("Password incorrect!"));
        return false;
    }

    return true;
}

void MainWindow::statusBarMessage(const QString& msg, int timeout)
{
    mutex.lock();
    statusBar()->showMessage(msg, timeout);
    mutex.unlock();
}

void MainWindow::save()
{
    ui.scriptPage->save();
    ScriptManager::instance()->save("scripts.json");
    SettingManager::instance()->save("config.dat");


    this->log(tr("Config saved."), LogPage::System);
}

void MainWindow::exit()
{
    if (!ui.scriptPage->isScriptRunning())
    {
        if (checkPassword()) {
            if (ProcessProtecter::isProtected())
                ProcessProtecter::unprotect();
            save();
            QApplication::exit(0);
        }
    }
    else {
        QMessageBox::warning(nullptr, tr("Note"), tr("A script is still running!Exiting is banned!"));
        this->log(tr("Exiting operation refused."), LogPage::System);
    }
}
