#pragma once

#include <qmainwindow.h>

class QLabel;
class QUrl;
class QStringList;
class QTimer;

namespace Ui {
    class ConcerntratingBrowser;
}

class ConcerntratingBrowser:
    public QMainWindow
{
    Q_OBJECT
public:
    ConcerntratingBrowser(QWidget* parent = Q_NULLPTR);

    void setAllowedHosts(const QStringList& hosts);
    void setDefaultUrl(const QUrl& url);

    void resetAllowedHosts();
    void resetDefaultUrl();

    QUrl currentUrl()const;

    bool isOpened()const { return _isOpened; }

public slots:
    void back();
    void forward();
    void load(const QUrl& url);

private slots:
    void urlFinishEditing();

private:
    Ui::ConcerntratingBrowser *ui;
    QLabel* timeLabel;

    QTimer* timer;

    bool _isOpened;

protected:
    void showEvent(QShowEvent* e);
};
