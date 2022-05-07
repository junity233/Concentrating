#include "WebView.h"
#include "TabWidget.h"

WebView::WebView(QWidget *parent)
	: QWebEngineView(parent)
{
}

WebView::~WebView()
{
}

void WebView::setTabWidget(TabWidget* tabWidget)
{
	_tabWidget = tabWidget;
}

WebView* WebView::createWindow(QWebEnginePage::WebWindowType type)
{
	switch (type) {
	case QWebEnginePage::WebBrowserTab:
		return _tabWidget->createTab();
	case QWebEnginePage::WebBrowserBackgroundTab:
		return _tabWidget->createBackgroundTab();
	case QWebEnginePage::WebBrowserWindow:
		return this;
	default:
		return Q_NULLPTR;
	}
}
