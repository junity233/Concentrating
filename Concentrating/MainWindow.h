#pragma once

#include <QtWidgets/QMainWindow>
#include <qsystemtrayicon.h>

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

signals:
    void initializeFinished();

private slots:
    void scriptRunFailed(const QString& reason);
    void scriptRunFinished(bool exitCode);
    void scriptAboutToRun(int idx);

    void systemTrayActived(QSystemTrayIcon::ActivationReason reason);

    void save();

    void exit();

protected:
    void closeEvent(QCloseEvent* event);

private:
    void setupSystemTray();
    void showWindow();

    void setupBrowser();
    void setupScriptPage();
    void setupLuaBinder();
private:
    Ui::MainWindowClass ui;

    QSystemTrayIcon* _systemTray;
    QTimer* _timer;
    QMenu *_menu;
    ConcerntratingBrowser* _browser;
    static MainWindow* _instance;
};
