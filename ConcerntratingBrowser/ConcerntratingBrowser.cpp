#include "ConcerntratingBrowser.h"
#include "ui_ConcerntratingBrowser.h"
#include <QtPlatformHeaders\qwindowswindowfunctions.h>
#include <qlabel.h>
#include <qtimer.h>
#include <qdatetime.h>
#include <Windows.h>

#include "DesktopHelper.h"
#include "TabWidget.h"

ConcerntratingBrowser::ConcerntratingBrowser(QWidget *parent)
	:QMainWindow(parent)
{
	ui = new Ui::ConcerntratingBrowser;
	ui->setupUi(this);

	timeLabel = new QLabel(this);

	ui->statusbar->addWidget(timeLabel);

	timer = new QTimer(this);
	timer->setInterval(1000);
	timer->start();

	connect(timer, &QTimer::timeout, [this]() {
		timeLabel->setText(QDateTime::currentDateTime().toString("yy/MM/dd hh:mm:ss"));
		});

	setWindowFlag(Qt::Window, true);
	setWindowFlag(Qt::WindowStaysOnTopHint);
	setWindowFlag(Qt::FramelessWindowHint, true);

	connect(ui->tabWidget, &TabWidget::urlChanged, [this](const QUrl& url) {
		this->ui->url->setText(url.toString());
		});

	_isOpened = true;

}

void ConcerntratingBrowser::setAllowedHosts(const QStringList& hosts)
{
	ui->tabWidget->setAllowedUrls(hosts);
}

void ConcerntratingBrowser::setDefaultUrl(const QUrl& url)
{
	ui->tabWidget->setDefaultUrl(url);
}

void ConcerntratingBrowser::resetAllowedHosts()
{
	setAllowedHosts(QStringList());
}

void ConcerntratingBrowser::resetDefaultUrl()
{
	setDefaultUrl(QUrl::fromUserInput("about:blank"));
}

QUrl ConcerntratingBrowser::currentUrl() const
{
	return ui->tabWidget->currentUrl();
}

void ConcerntratingBrowser::load(const QUrl& url)
{
	if (ui->tabWidget->currentView() == Q_NULLPTR)
		ui->tabWidget->createTab();

	ui->tabWidget->setCurrentUrl(url);
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
	QMainWindow::showEvent(e);
	QWindowsWindowFunctions::setHasBorderInFullScreen(this->windowHandle(), true);

	_isOpened = true;
}

void ConcerntratingBrowser::closeEvent(QCloseEvent* e)
{
	_isOpened = false;
	QMainWindow::closeEvent(e);
}

void ConcerntratingBrowser::focusOutEvent(QFocusEvent* e)
{
	if (_isOpened) {
		showFullScreen();

		DesktopHelper::Desktop inputDesk = DesktopHelper::GetCurrentDesktop(),
			threadDesk = DesktopHelper::GetThreadDesktop();

		if (inputDesk.name() != threadDesk.name()) {
			threadDesk.switchTo();
		}
	}
}

bool ConcerntratingBrowser::event(QEvent* e)
{
	if (e->type() == QEvent::Hide) {
		_isOpened = false;
	}

	return QMainWindow::event(e);
}
