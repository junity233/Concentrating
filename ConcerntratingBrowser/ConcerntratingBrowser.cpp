#include "ConcerntratingBrowser.h"
#include "ui_ConcerntratingBrowser.h"
#include <QtPlatformHeaders\qwindowswindowfunctions.h>

#include "TabWidget.h"

ConcerntratingBrowser::ConcerntratingBrowser(QWidget *parent)
	:QMainWindow(parent)
{
	ui = new Ui::ConcerntratingBrowser;
	ui->setupUi(this);


	setWindowFlag(Qt::Window, true);
	setWindowFlag(Qt::WindowStaysOnTopHint);
	setWindowFlag(Qt::FramelessWindowHint, true);

	connect(ui->tabWidget, &TabWidget::urlChanged, [this](const QUrl& url) {
		this->ui->url->setText(url.toString());
		});

	_shouldHide = true;

}

void ConcerntratingBrowser::setAllowedHosts(const QStringList& hosts)
{
	ui->tabWidget->setAllowedUrls(hosts);
}

void ConcerntratingBrowser::setDefaultUrl(const QUrl& url)
{
	ui->tabWidget->setDefaultUrl(url);
}

QUrl ConcerntratingBrowser::currentUrl() const
{
	return ui->tabWidget->currentUrl();
}

void ConcerntratingBrowser::forward()
{
	ui->tabWidget->forward();
}

void ConcerntratingBrowser::back()
{
	ui->tabWidget->back();
}

void ConcerntratingBrowser::urlFinishEditing()
{
	QUrl url = QUrl::fromUserInput(ui->url->text());
	if (url.isValid())
		ui->tabWidget->setCurrentUrl(url);
}

void ConcerntratingBrowser::showEvent(QShowEvent* e)
{
	QWidget::showEvent(e);
	QWindowsWindowFunctions::setHasBorderInFullScreen(this->windowHandle(), true);
}
