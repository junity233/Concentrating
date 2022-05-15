#pragma once

#include <QAbstractTableModel>
#include "ScheduleManager.h"

class ScheduleTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	explicit ScheduleTableModel(QObject *parent);
	~ScheduleTableModel();

	int rowCount(const QModelIndex& index)const;
	int columnCount(const QModelIndex& index)const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

	bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex())override;
	bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex())override;

public slots:
	void reset();
	void store()const;


private:
	QVector<ScheduleManager::Task> _data;
};
