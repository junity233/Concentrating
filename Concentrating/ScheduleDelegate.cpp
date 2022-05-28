#include "ScheduleDelegate.h"
#include "ScriptComboBox.h"
#include <qlineedit.h>

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
		return new ScriptComboBox(parent);
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
		ScriptComboBox* combo = qobject_cast<ScriptComboBox*>(editor);
		if (combo) {
			combo->setCurrentIndex(index.data().toInt());
		}
		else QItemDelegate::setEditorData(editor, index);
	}
	else QItemDelegate::setEditorData(editor, index);
}

void ScheduleDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	if (index.column() == 1) {
		ScriptComboBox* combo = qobject_cast<ScriptComboBox*>(editor);
		if (combo) {
			model->setData(index, combo->currentIndex());
			return;
		}
	}

	QItemDelegate::setModelData(editor, model, index);
}

void ScheduleDelegate::updateEditorGeometry(QWidget* editor, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
	editor->setGeometry(option.rect);
}
