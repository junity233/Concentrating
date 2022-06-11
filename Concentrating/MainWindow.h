#pragma once

#include <QtWidgets/QMainWindow>
#include <qsystemtrayicon.h>
#include <qmutex.h>

#include "ui_MainWindow.h"

class ConcerntratingBrowser;
class QMenu;
class QAction;
class QTimer;
class LuaScriptRunnerPool;
class ScriptPage;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

    ConcerntratingBrowser* browser()const {
        return _browser;
    };

    static MainWindow* instance() { return _instance; }

    QSystemTrayIcon* systemTray()const {
        return _systemTray;
    }

public slots:
    void log(const QString& msg, LogPage::Role role);
    void statusBarMessage(const QString& msg, int timeout = 2000);

    void runScript(const QString& name, const QString& code);
    void runScript(const QUrl& path);

signals:
    void initializeFinished();

private slots:
    void systemTrayActived(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void saveConfig();

    void saveScript();
    void scriptSaveAll();
    void scriptSaveAs();

    void openScript();
    void newScript();
    void closeScript();
    void closeScript(int idx);

    void exit();

    void loadLastScripts();
    void saveLastScripts();

protected:
    void closeEvent(QCloseEvent* event);

private:
    void setupSystemTray();

    void setupBrowser();
    void setupLuaBinder();
    void setupScriptRunnerPool();

    bool checkPassword();

    void updateTitleForScript(ScriptPage* page);

    ScriptPage* currentScriptPage() const;
    ScriptPage* scriptPage(int idx)const;

    void setupScriptPage(ScriptPage* page);
    ScriptPage* createScriptPage();

    void openScript(const QUrl& path);


private:
    Ui::MainWindowClass ui;

    QSystemTrayIcon* _systemTray;
    QTimer* _timer;
    QMenu *_menu;
    ConcerntratingBrowser* _browser;
    static MainWindow* _instance;

    LuaScriptRunnerPool* _scriptRunnerPool;
};
