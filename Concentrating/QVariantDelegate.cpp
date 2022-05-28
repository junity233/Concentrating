#include "QVariantDelegate.h"
#include "QVariantTypeComboBox.h"
#include <qdebug.h>

QVariantDelegate::QVariantDelegate(QObject *parent)
	: QItemDelegate(parent)
{
}

QVariantDelegate::~QVariantDelegate()
{
}

QWidget* QVariantDelegate::createEditor(QWidget* parent, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
		QVariantTypeComboBox* combo = new QVariantTypeComboBox(parent);

		combo->setVaildTypes({ QVariant::Map,
		QVariant::List,
		QVariant::Int,
		QVariant::Double,
		QVariant::Bool,
		QVariant::String,
		QVariant::Time,
		QVariant::Date,
		QVariant::DateTime,
		QVariant::Url });

		return combo;
}

void QVariantDelegate::setEditorData(QWidget* editor, const QModelIndex& index) const
{
	QVariantTypeComboBox* combo = qobject_cast<QVariantTypeComboBox*>(editor);

	if (combo) {
		qDebug() << index.data();
		combo->setType((QVariant::Type)index.data(Qt::EditRole).toInt());
	}
	else setEditorData(editor, index);
}

void QVariantDelegate::setModelData(QWidget* editor, QAbstractItemModel* model, const QModelIndex& index) const
{
	QVariantTypeComboBox* combo = qobject_cast<QVariantTypeComboBox*>(editor);

	if (combo) {
		model->setData(index, (int)combo->type());
	}
	else setEditorData(editor, index);
}
