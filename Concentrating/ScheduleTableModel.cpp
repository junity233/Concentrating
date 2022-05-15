#include "ScheduleTableModel.h"
#include "ScriptManager.h"

ScheduleTableModel::ScheduleTableModel(QObject *parent)
	: QAbstractTableModel(parent)
{
	reset();
}

ScheduleTableModel::~ScheduleTableModel()
{
}

int ScheduleTableModel::rowCount(const QModelIndex& index) const
{
	return _data.size();
}

int ScheduleTableModel::columnCount(const QModelIndex& index) const
{
	return 2;
}

QVariant ScheduleTableModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid())
		return QVariant();
	if (role == Qt::EditRole || role == Qt::DisplayRole) {
		int row = index.row();
		int column = index.column();

		if (row < 0 || row >= _data.size())
			return QVariant();

		if (column == 0) {
			if (role == Qt::DisplayRole)
				return _data[row].time.toString("hh:mm:ss");
			else return _data[row].time;
		}
		else if (column == 1)
			return ScriptManager::instance()->script(_data[row].script).name;
	}

	return QVariant();
}

QVariant ScheduleTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
		if (section == 0)
			return tr("Time");
		else if (section == 1)
			return tr("Script");
	}

	return QVariant();
}

Qt::ItemFlags ScheduleTableModel::flags(const QModelIndex& index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool ScheduleTableModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (!index.isValid())
		return false;

	if (role == Qt::EditRole) {
		int row = index.row();
		int colum = index.column();

		if (row < 0 || row >= _data.size())
			return false;

		if (colum == 0) {
			_data[row].time = value.toTime();
			return true;
		}
		else if (colum == 1) {
			_data[row].script = value.toInt();
			return true;
		}
	}

	return false;
}

bool ScheduleTableModel::insertRows(int row, int count, const QModelIndex& parent)
{
	if (row == -1)
		row = 0;
	beginInsertRows(parent, row, row + count - 1);
	_data.insert(row, count, ScheduleManager::Task());
	endInsertRows();

	return true;
}

bool ScheduleTableModel::removeRows(int row, int count, const QModelIndex& parent)
{
	if (row + count - 1 >= _data.size())
		return false;
	beginRemoveRows(parent, row, row + count - 1);
	_data.remove(row, count);
	endRemoveRows();
	return true;
}

void ScheduleTableModel::store() const
{
	ScheduleManager::instance()->setTasks(_data);
	ScheduleManager::instance()->store();
}
void ScheduleTableModel::reset()
{
	beginResetModel();
	ScheduleManager::instance()->load();
	_data = ScheduleManager::instance()->tasks();
	endResetModel();
}
