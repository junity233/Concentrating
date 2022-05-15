#include "SettingTableModel.h"
#include "SettingManager.h"

SettingTableModel::SettingTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	_data = SettingManager::instance()->data();
}

SettingTableModel::~SettingTableModel()
{
}

int SettingTableModel::rowCount(const QModelIndex& index) const
{
	return _data.count();
}

int SettingTableModel::columnCount(const QModelIndex& index) const
{
	return 2;
}

QVariant SettingTableModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole || role == Qt::EditRole) {
		int row = index.row();
		int column = index.column();
		if (row >= _data.size())
			return QVariant();

		QString key = _data.keys()[row];

		if (column == 0) {
			return key;
		}
		else if (column == 1) {
			const QVariant& v = _data[key];
			auto type = v.type();

			switch (type) {
			case QVariant::String:
			case QVariant::Int:
			case QVariant::Double:
			case QVariant::Bool:
			case QVariant::Time:
			case QVariant::Url:
				return v;
			default:
				return QString("[%1]").arg(v.typeName());
			}
		}
	}
	return QVariant();
}

QVariant SettingTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0)
			return tr("Key");
		else if (section == 1)
			return tr("Value");
		return QVariant();
	}

	return QVariant();
}

Qt::ItemFlags SettingTableModel::flags(const QModelIndex& index) const
{
	int row = index.row();
	int column = index.column();

	QString key = _data.keys()[row];
	if(column==0)
		return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
	else if (column == 1) {
		auto type = _data[key].type();
		switch (type) {
		case QVariant::String:
		case QVariant::Int:
		case QVariant::Double:
		case QVariant::Bool:
		case QVariant::Time:
		case QVariant::Url:
			return Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable;
		default:
			return Qt::ItemIsSelectable;
		}
	}

	return Qt::ItemFlags();
}

bool SettingTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	int row = index.row();
	int column = index.column();

	if (role == Qt::EditRole) {
		QString key = _data.keys()[row];

		if (column == 0) {
			QString newKey = value.toString();
			if (newKey.isNull() || newKey.isEmpty() || _data.contains(newKey))
				return false;

			QVariant data = _data[key];
			_data.remove(key);
			_data.insert(newKey, data);
			return true;
		}
		else if (column == 1) {
			_data[key] = value;
			return true;
		}
	}
	return false;
}


void SettingTableModel::reset()
{
	beginResetModel();
	_data = SettingManager::instance()->data();
	endResetModel();
}

void SettingTableModel::store() const
{
	SettingManager::instance()->setData(_data);
}

void SettingTableModel::newSetting()
{
	beginInsertRows(QModelIndex(), _data.count(), _data.count());

	int idx = 0;
	while (_data.contains(tr("New Item %1").arg(idx)))
		idx++;

	_data.insert(tr("New Item %1").arg(idx), QVariant());

	endInsertRows();
}

void SettingTableModel::deleteSetting(int row)
{
	if (row >= _data.size())
		return;

	beginRemoveRows(QModelIndex(), row, row);
	QString key = _data.keys()[row];
	_data.remove(key);

	endRemoveRows();
}
