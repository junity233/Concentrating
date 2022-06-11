#include "ScheduleDelegate.h"
#include <qlineedit.h>
#include "PathEdit.h"

ScheduleDelegate::ScheduleDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

ScheduleDelegate::~ScheduleDelegate()
{
}

QWidget* ScheduleDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	if (index.column() == 1) {
		PathEdit* editor = new PathEdit(parent);

		editor->setFilter(tr("Lua script (*.lua)"));
		editor->setMode(PathEdit::Open);
		editor->setTitle(tr("Select script..."));
		editor->setDefaultPath("scripts");
		
		return editor;
	}
	else if (index.column() == 0) {
		auto editor = new QLineEdit(parent);
		return editor;
	}

	return QItemDelegate::createEditor(parent, option, index);
}

void ScheduleDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	if (index.column() == 1) {
		PathEdit* path = qobject_cast<PathEdit*>(editor);
		if (path) {
			path->setPath(index.data().toString());
		}
		else QItemDelegate::setEditorData(editor, index);
	}
	else QItemDelegate::setEditorData(editor, index);
}

void ScheduleDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() == 1) {
		PathEdit* path = qobject_cast<PathEdit*>(editor);
		if (path) {
			model->setData(index, path->path());
			return;
		}
	}

	QItemDelegate::setModelData(editor, model, index);
}

void ScheduleDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
