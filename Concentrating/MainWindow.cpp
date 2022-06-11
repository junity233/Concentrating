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
#include <qurl.h>
#include <qresource.h>
#include <qtimer.h>
#include <qinputdialog.h>
#include <qfiledialog.h>

#include "MusicPlayer.h"
#include "MethodInvoker.hpp"
#include "SettingManager.h"
#include "ScriptManager.h"
#include "ScriptPage.h"
#include "ScheduleManager.h"
#include "LuaBinding.h"
#include "LuaScriptRunnerPool.h"
#include "ProcessProtecter.h"
#include "ProcessHelper.h"
#include "MouseHelper.h"
#include "KeyboardHelper.h"
#include "TextSpeaker.h"

#include "version.h"

MainWindow* MainWindow::_instance = Q_NULLPTR;

#define foreach_scripts(iterator) for(int iterator = 0; iterator < ui.scripts->count(); iterator ++)

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
{
    SettingManager::instance()->setPath(QUrl::fromLocalFile("config.dat"));
    SettingManager::instance()->load();

    _instance = this;

    setupBrowser();

    ui.setupUi(this);

    this->log("Concentrating(" COMPILE_DATETIME ") [" COMPILER "] on " SYSTEM, LogPage::System);
    this->log(tr("Initialization begin."), LogPage::System);
    this->log(tr("Setup script system."), LogPage::System);

    setupLuaBinder();
    setupScriptRunnerPool();

    connect(ui.pages, &QTabWidget::currentChanged, [this](int idx) {
        if (idx == 1)
            ui.settingPage->reset(-1);
        });

    connect(ui.scripts, &QTabWidget::currentChanged, [this](int idx) {
        updateTitleForScript(scriptPage(idx));
        });

    this->log(tr("Setup system tray."), LogPage::System);
    setupSystemTray();

    this->log(tr("Setup schedule."), LogPage::System);

    _timer = new QTimer(this);
    _timer->setInterval(1000);
    _timer->start();
    connect(_timer, &QTimer::timeout, ScheduleManager::instance(), &ScheduleManager::schedule);
    connect(ScheduleManager::instance(), &ScheduleManager::runScript, this, qOverload<const QUrl&>(&MainWindow::runScript));

    this->log(tr("Setup process protection."), LogPage::System);
    ProcessProtecter::protect(ProcessHelper::currentPid());

    loadLastScripts();

    QDir dir = QDir("scripts");
    if (!dir.exists())
        QDir().mkdir("scripts");


    emit initializeFinished();
}

void MainWindow::log(const QString& msg, LogPage::Role role) {
    ui.logPage->log(msg, role);
}

void MainWindow::systemTrayActived(QSystemTrayIcon::ActivationReason reason)
{
    switch (reason) {
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
    saveConfig();
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

    MusicPlayer::load();
    MusicPlayer::setPath(QDir::current().absoluteFilePath("music"));
}

void MainWindow::setupScriptRunnerPool()
{
    _scriptRunnerPool = new LuaScriptRunnerPool(this);

    connect(_scriptRunnerPool, &LuaScriptRunnerPool::finished, this, [this](const QString& name, bool exitCode) {
        QString msg;

        if (!exitCode)
            msg = tr("Script \"%1\" run completely.").arg(name);
        else
            msg = tr("Script \"%1\" run failed.").arg(name);

        log(msg, LogPage::System);
        statusBarMessage(msg);

        if (SettingManager::instance()->value("system.notice_script_finished", true).toBool())
            systemTray()->showMessage(tr("Notice"), msg);

        });

    connect(_scriptRunnerPool, &LuaScriptRunnerPool::failed, this, [this](const QString& name, const QString& reason) {
        this->log(reason, LogPage::Lua);

        if (SettingManager::instance()->value("system.notice_script_failed", true).toBool())
            this->systemTray()->showMessage(tr("Notice"), tr("Script \"%1\" runs failed!").arg(name));
        });
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

void MainWindow::updateTitleForScript(ScriptPage* page)
{
    if (page) {
        QString name = (page->path().isEmpty()) ? (tr("New script")) : (page->path().fileName());
        QString namePart = QStringLiteral("%1 %2").arg(name).arg(page->modified() ? '*' : ' ');

        ui.scripts->setTabText(ui.scripts->indexOf(page), namePart);

        if (page == ui.scripts->currentWidget())
            setWindowTitle(tr("Concentrating -- ") + namePart);
    }
    else if (page == ui.scripts->currentWidget())
        setWindowTitle(tr("Concentrating"));
}

ScriptPage* MainWindow::currentScriptPage() const
{
    return qobject_cast<ScriptPage*>(ui.scripts->currentWidget());
}

ScriptPage* MainWindow::scriptPage(int idx) const
{
    return qobject_cast<ScriptPage*>(ui.scripts->widget(idx));
}

void MainWindow::setupScriptPage(ScriptPage* page)
{
    connect(page, &ScriptPage::run, this, [this, page]() {
        runScript(page->path().fileName(), page->code());
        });

    connect(page, &ScriptPage::modifyChanged, this, [this, page](bool modified) {
            updateTitleForScript(page);
        });
    connect(page, &ScriptPage::pathChanged, this, [this, page](const QUrl& url) {
        updateTitleForScript(page);
        });
}

ScriptPage* MainWindow::createScriptPage()
{
    ScriptPage* page = new ScriptPage(this);
    setupScriptPage(page);

    return page;
}

void MainWindow::statusBarMessage(const QString& msg, int timeout)
{
    statusBar()->showMessage(msg, timeout);
}

void MainWindow::runScript(const QString& name, const QString& code)
{
    QString msg = tr(R"(Script "%1" start to run!)").arg(name);

    if (SettingManager::instance()->value("system.notice_script_start", true).toBool()) {
        systemTray()->showMessage(tr("Notice"), msg);
    }

    log(msg, LogPage::System);
    statusBar()->showMessage(msg);

    _scriptRunnerPool->run(name, code);
}

void MainWindow::runScript(const QUrl& path)
{
    QFile f(path.toLocalFile());

    f.open(QIODevice::ReadOnly);

    if (!f.isOpen()) {
        this->log(tr("Run script \"%1\" failed:Cannot open file!").arg(path.toLocalFile()), LogPage::System);

        if (settingManager->value("system.notice_script_failed").toBool()) {
            _systemTray->showMessage(
                tr("Error"),
                tr("Run script \"%1\" failed:Cannot open file!").arg(path.toLocalFile())
            );
        }

        return;
    }

    QString code = f.readAll();
    f.close();

    runScript(path.fileName(), code);
}

void MainWindow::saveConfig()
{
    SettingManager::instance()->save();


    this->log(tr("Config saved."), LogPage::System);
}

void MainWindow::saveScript()
{
    currentScriptPage()->save();
}

void MainWindow::scriptSaveAll()
{
    for (int i = 0; i < ui.scripts->count(); i++) {
        scriptPage(i)->save();
    }
}

void MainWindow::scriptSaveAs()
{
    currentScriptPage()->saveAs();
}

void MainWindow::openScript(const QUrl& path)
{
    foreach_scripts(i) {
        ScriptPage* page = scriptPage(i);
        if(page)
            if (path == page->path())
                return;
    }

    QFile f(path.toLocalFile());

    f.open(QIODevice::ReadOnly);

    if (!f.isOpen()) {
        QMessageBox::critical(
            this,
            tr("Error"),
            tr("Open script %1 failed!").arg(path.toLocalFile())
        );
        return;
    }
    ScriptPage* page = createScriptPage();

    QString code = QString::fromUtf8(f.readAll());
    page->setCode(code);
    page->setPath(path);

    addScriptTab(page);

    f.close();
}

void MainWindow::openScript()
{
    QUrl path = QFileDialog::getOpenFileUrl(
        this,
        tr("Open Script"),
        QUrl::fromLocalFile("scripts"),
        tr("Lua script (*.lua)")
    );

    if (path.isEmpty())
        return;

    openScript(path);
}

void MainWindow::newScript()
{
    ScriptPage* newScriptPage = createScriptPage();

    addScriptTab(newScriptPage);
}

void MainWindow::addScriptTab(ScriptPage* newScriptPage)
{
    int idx = ui.scripts->addTab(newScriptPage, tr("New Script"));
    ui.scripts->setCurrentIndex(idx);
    ui.scripts->setTabIcon(idx, QIcon(":/resource/file-code.png"));
}

void MainWindow::closeScript()
{
    closeScript(ui.scripts->currentIndex());
}

void MainWindow::closeScript(int idx)
{
    ScriptPage* page = scriptPage(idx);

    if (page->modified()) {
        int res = QMessageBox::question(
            this,
            tr("Note"),
            tr("File doesn't saved.Do you want to continue?"),
            QMessageBox::Yes,
            QMessageBox::Cancel,
            QMessageBox::Save
        );

        if (res == QMessageBox::Cancel)
            return;
        else if (res == QMessageBox::Save)
            page->save();

    }

    ui.scripts->removeTab(ui.scripts->indexOf(page));
    page->close();
    page->deleteLater();
}

void MainWindow::exit()
{
    if (_scriptRunnerPool->isFinished())
    {
        if (checkPassword()) {
            if (ProcessProtecter::isProtected())
                ProcessProtecter::unprotect();

            saveConfig();
            saveLastScripts();

            QApplication::exit(0);
        }
    }
    else {
        QMessageBox::warning(nullptr, tr("Note"), tr("A script is still running!Exiting is banned!"));
        this->log(tr("Exiting operation refused."), LogPage::System);
    }
}

void MainWindow::loadLastScripts()
{
    QFile f("last.dat");

    f.open(QIODevice::ReadOnly);

    if (!f.isOpen()) {
        this->log(tr("Load last scripts failed:Cannot open file!"), LogPage::System);
        return;
    }

    QList<QUrl> scripts;

    QDataStream s(&f);

    s >> scripts;

    f.close();

    for (const QUrl& script : scripts)
    {
        if(script.isValid())
            openScript(script);
    }
}

void MainWindow::saveLastScripts()
{
    QFile f("last.dat");
    f.open(QIODevice::WriteOnly);

    if (!f.isOpen()) {
        this->log(tr("Save last scripts failed:Cannot open file!"), LogPage::System);
        return;
    }

    QDataStream s(&f);

    QList<QUrl> scripts;

    for (int i = 0; i < ui.scripts->count(); i++) {
        ScriptPage* page = scriptPage(i);

        if (page && page->path().isValid())
            scripts.append(page->path());
        else
            this->log(tr("Save last script failed:Cannot get path of page %1").arg(i), LogPage::System);
    }

    s << scripts;
    f.close();
}
