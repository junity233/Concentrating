#include "PathEdit.h"
#include <qfiledialog.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <qlayout.h>

PathEdit::PathEdit(QWidget *parent)
	: QWidget(parent)
{
	_path = new QLineEdit(this);
	_select = new QPushButton(QStringLiteral("..."), this);

	QHBoxLayout* layout = new QHBoxLayout(this);

	layout->addWidget(_path);
	layout->addWidget(_select);

	layout->setContentsMargins(0, 0, 0, 0);

	setLayout(layout);

	connect(_select, &QPushButton::clicked, this, &PathEdit::selectPath);

	_path->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
	_select->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Expanding);

	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}

PathEdit::~PathEdit()
{
}

void PathEdit::selectPath() {
	QString newPath;
	if (_mode == Mode::Open)
		newPath = QFileDialog::getOpenFileName(
			this,
			_title,
			_defaultPath,
			_filter
		);
	else
		newPath = QFileDialog::getSaveFileName(
			this,
			_title,
			_defaultPath,
			_filter
		);

	if (newPath.isNull())
		return;

	_path->setText(newPath);
	emit pathChanged(newPath);

}
