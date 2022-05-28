#pragma once

#include <QtWidgets/QMainWindow>
#include <qsystemtrayicon.h>
#include <qmutex.h>

#include "ui_MainWindow.h"

class ConcerntratingBrowser;
class QMenu;
class QAction;
class QTimer;

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

signals:
    void initializeFinished();

private slots:
    void systemTrayActived(QSystemTrayIcon::ActivationReason reason);
    void showWindow();
    void save();

    void exit();

protected:
    void closeEvent(QCloseEvent* event);

private:
    void setupSystemTray();

    void setupBrowser();
    void setupLuaBinder();

    bool checkPassword();
private:
    Ui::MainWindowClass ui;

    QSystemTrayIcon* _systemTray;
    QTimer* _timer;
    QMenu *_menu;
    ConcerntratingBrowser* _browser;
    static MainWindow* _instance;

    QMutex mutex;
};
