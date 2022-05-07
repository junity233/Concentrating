#pragma once

#include <QTabWidget>
#include <qvector.h>
#include <qurl.h>

class WebView;
class QRegExp;

typedef QSet<QUrl> QUrlSet;

class TabWidget : public QTabWidget
{
	Q_OBJECT

public:
	TabWidget(QWidget *parent=Q_NULLPTR);
	~TabWidget();

	WebView* createTab();
	WebView* createBackgroundTab();

	QUrl defaultUrl()const { return _defaultUrl; }
	void setDefaultUrl(const QUrl& url);

	QStringList allowedUrls()const { return _allowedUrls; }
	void setAllowedUrls(const QStringList& urlList);

	QUrl currentUrl()const;
	void setCurrentUrl(const QUrl& url);

	void closeTab(int index);

	WebView* currentView() const;

signals:
	void urlChanged(const QUrl& url);

public slots:
	void forward();
	void back();
	void refresh();
	void closeCurrent();
	void newTab();

private:
	void setupView(WebView* view);
	bool isUrlAllowed(const QUrl& url);

private:
	QVector<WebView*> webViews;

	QUrl _defaultUrl;

	QStringList _allowedUrls;
	QList<QRegExp> _allowedUrlRegExps;
};
