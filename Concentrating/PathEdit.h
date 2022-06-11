#pragma once

#include <QWidget>
#include <qlineedit.h>

class QPushButton;

class PathEdit : public QWidget
{
	Q_OBJECT

public:
	PathEdit(QWidget *parent = Q_NULLPTR);
	~PathEdit();

	enum Mode {
		Open,
		Save
	};

	inline QString title()const { return _title; }
	inline void setTitle(const QString& title) { _title = title; }

	inline Mode mode()const { return _mode; }
	inline void setMode(Mode mode) { _mode = mode; }

	inline QString filter()const { return _filter; }
	inline void setFilter(const QString& filter) { _filter = filter; }

	inline QString path()const { return _path->text(); }
	inline void setPath(const QString& path) { _path->setText(path); }

	inline QString defaultPath()const { return _defaultPath; }
	inline void setDefaultPath(const QString& defaultPath) { _defaultPath = defaultPath; }

signals:
	void pathChanged(const QString& path);

public slots:
	void selectPath();

private:
	QLineEdit* _path;
	QPushButton* _select;

	QString _title;
	QString _filter;
	QString _defaultPath;
	Mode _mode;
};
