#pragma once

#include <qmainwindow.h>

class QUrl;
class QStringList;

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

    QUrl currentUrl()const;

public slots:
    void back();
    void forward();
    void urlFinishEditing();

private:
    Ui::ConcerntratingBrowser *ui;

    bool _shouldHide;

protected:
    void showEvent(QShowEvent* e);
};
