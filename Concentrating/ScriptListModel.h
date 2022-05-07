#pragma once

#include <QAbstractListModel>

class ScriptListModel : public QAbstractListModel
{
	Q_OBJECT

public:
	ScriptListModel(QObject *parent);
	~ScriptListModel();
	
	int rowCount(const QModelIndex& parent = QModelIndex())const override;
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)const override;
	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole)const override;
	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole)override;
	Qt::ItemFlags flags(const QModelIndex& index)const override;

	QModelIndex append();
	void remove(const QModelIndex& index);
};
