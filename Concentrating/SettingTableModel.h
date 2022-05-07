#pragma once

#include <QAbstractTableModel>

class SettingTableModel : public QAbstractTableModel
{
	Q_OBJECT

public:
	SettingTableModel(QObject *parent);
	~SettingTableModel();

	int rowCount(const QModelIndex& index)const;
	int columnCount(const QModelIndex& index)const;

	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole)const;

	QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const;

	Qt::ItemFlags flags(const QModelIndex& index) const;

	bool setData(const QModelIndex& index, const QVariant& value, int role = Qt::EditRole);

public slots:
	void reset();
	void store()const;
	void newSetting();
	void deleteSetting(int row);

private:
	QVariantMap _data;
};
