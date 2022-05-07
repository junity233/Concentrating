#pragma once

#include <QtWidgets/QMainWindow>

#include "ui_MainWindow.h"

class ConcerntratingBrowser;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = Q_NULLPTR);

    ConcerntratingBrowser* browser()const {
        return _browser;
    };

    static MainWindow* instance() { return _instance; }

private slots:
    void scriptRunFailed(const QString& reason);
    void scriptRunFinished(int exitCode);

protected:
    void closeEvent(QCloseEvent* event);

private:
    Ui::MainWindowClass ui;

    ConcerntratingBrowser* _browser;
    static MainWindow* _instance;
};
