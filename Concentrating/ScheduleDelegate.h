#pragma once

#include <QItemDelegate>

class ScheduleDelegate : public QItemDelegate
{
	Q_OBJECT

public:
	explicit ScheduleDelegate(QObject *parent);
	~ScheduleDelegate();

    QWidget* createEditor(QWidget* parent,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;
    void setEditorData(QWidget* editor,
        const QModelIndex& index) const override;
    void setModelData(QWidget* editor,
        QAbstractItemModel* model, const QModelIndex& index) const override;
    void updateEditorGeometry(QWidget* editor,
        const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};
