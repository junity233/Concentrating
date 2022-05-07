#include "TabWidget.h"
#include "WebView.h"
#include <qicon.h>
#include <qurl.h>
#include <qwebenginehistory.h>
#include <qtabbar.h>
#include <qregexp.h>

TabWidget::TabWidget(QWidget *parent)
	: QTabWidget(parent)
{
	QTabBar* tabBar = this->tabBar();
	tabBar->setTabsClosable(true);
	tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectPreviousTab);
	tabBar->setMovable(true);
	tabBar->setContextMenuPolicy(Qt::CustomContextMenu);

	connect(this, &QTabWidget::currentChanged, [this](int idx) {
		WebView* view = qobject_cast<WebView*>(widget(idx));
		if(view)
			emit urlChanged(view->url());
		});

	connect(tabBar, &QTabBar::tabCloseRequested, this, &TabWidget::closeTab);

	newTab();
}

TabWidget::~TabWidget()
{
}

void TabWidget::setAllowedUrls(const QStringList& urls)
{
	_allowedUrls = urls;
	_allowedUrlRegExps.clear();

	for (auto i : urls) {
		QRegExp reg(i);
		reg.setPatternSyntax(QRegExp::Wildcard);
		_allowedUrlRegExps.append(reg);
	}
}

QUrl TabWidget::currentUrl() const
{
	return currentView()->url();
}

void TabWidget::setCurrentUrl(const QUrl& url)
{
	if(isUrlAllowed(url))
		currentView()->setUrl(url);
}

void TabWidget::closeTab(int index)
{
	WebView *view = qobject_cast<WebView*>(widget(index));
	if (view !=Q_NULLPTR) {
		bool hasFocus = view->hasFocus();
		removeTab(index);
		if (hasFocus && count() > 0)
			currentView()->setFocus();
		if (count() == 0)
			createTab();
		view->deleteLater();
	}
}

WebView* TabWidget::currentView() const
{
	return qobject_cast<WebView*>(currentWidget());
}


WebView* TabWidget::createTab()
{
	WebView* view = createBackgroundTab();
	setCurrentWidget(view);
	return view;
}

WebView* TabWidget::createBackgroundTab()
{
	WebView* view = new WebView(this);
	this->webViews.append(view);


	view->setUrl(_defaultUrl);
	setupView(view);

	int idx = addTab(view, tr("Untitled"));

	view->resize(this->currentWidget()->size());
	view->show();

	return view;
}

void TabWidget::setDefaultUrl(const QUrl& url)
{
	_defaultUrl = url;
}

void TabWidget::forward()
{
	WebView* view = currentView();
	if (view)
		view->forward();
}

void TabWidget::back()
{

	WebView* view = currentView();
	if (view)
		view->back();
}

void TabWidget::refresh()
{
	WebView* view = currentView();
	if (view)
		view->reload();
}

void TabWidget::closeCurrent()
{
	closeTab(currentIndex());
}

void TabWidget::newTab()
{
	WebView* view = createTab();
	setCurrentWidget(view);
}

void TabWidget::setupView(WebView* view)
{
	view->setTabWidget(this);

	connect(view, &WebView::titleChanged, [this,view](const QString& title) {
		int idx = indexOf(view);
		if (idx != -1) {
			setTabText(idx, title);
			setTabToolTip(idx, title);
		}
		});

	connect(view, &WebView::iconChanged, [this, view](const QIcon& icon) {
		int idx = indexOf(view);
		if (idx != -1) {
			setTabIcon(idx, icon);
		}
		});

	connect(view, &WebView::urlChanged, [this, view](const QUrl& url) {
		if (!isUrlAllowed(view->url())) {
			if (view->history()->canGoBack())
				view->back();
			else closeTab(indexOf(view));
		}
		});

	//一部分网页会在打开后跳转到另一个网页，故只用loadStarted会检测不到
	connect(view, &WebView::urlChanged, [this, view](const QUrl& url) {
		if (!isUrlAllowed(view->url())) {
			if (view->history()->canGoBack())
				view->back();
			else closeTab(indexOf(view));
		}
		else {
			int index = this->indexOf(view);
			if (index != -1) {
				tabBar()->setTabData(index, url);
			}

			if (index == currentIndex()) {
				emit urlChanged(view->url());
			}
		}
		});
}

bool TabWidget::isUrlAllowed(const QUrl& url)
{

	if (url==QUrl::fromUserInput("about:blank"))
		return true;

	for (auto i : _allowedUrlRegExps) {
		if (i.exactMatch(url.host()))
			return true;
	}
	return false;
}
