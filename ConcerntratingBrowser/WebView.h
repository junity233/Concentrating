#pragma once

#include <QWebEngineView>

class TabWidget;

class WebView : public QWebEngineView
{
	Q_OBJECT

public:
	WebView(QWidget *parent);
	~WebView();

	void setTabWidget(TabWidget* tabWidget);
	TabWidget* tabWidget()const { return _tabWidget; }

protected:
	WebView* createWindow(QWebEnginePage::WebWindowType type);

private:
	TabWidget* _tabWidget;
};
