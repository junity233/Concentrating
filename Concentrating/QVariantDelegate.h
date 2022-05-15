#pragma once

#include <QItemDelegate>

class QVariantDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	QVariantDelegate(QObject *parent);
	~QVariantDelegate();


	QWidget* createEditor(QWidget* parent,
		const QStyleOptionViewItem& option, const QModelIndex& index) const override;
	void setEditorData(QWidget* editor,
		const QModelIndex& index) const override;
	void setModelData(QWidget* editor,
		QAbstractItemModel* model, const QModelIndex& index) const override;
};
/*

		
*/