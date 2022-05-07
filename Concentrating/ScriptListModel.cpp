#include "ScriptListModel.h"
#include "ScriptManager.h"
#include "qmap.h"

ScriptListModel::ScriptListModel(QObject *parent)
	: QAbstractListModel(parent)
{
}

ScriptListModel::~ScriptListModel()
{
}

int ScriptListModel::rowCount(const QModelIndex& parent) const
{
	return ScriptManager::instance()->scriptCount();
}

QVariant ScriptListModel::data(const QModelIndex& index, int role) const
{
	if (role == Qt::DisplayRole) {
		int idx = index.row();

		if (idx < ScriptManager::instance()->scriptCount()) {
			return ScriptManager::instance()->scripts()[idx].name;
		}
	}
	return QVariant();
}

QVariant ScriptListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
	return tr("Name");
}

bool ScriptListModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
	if (role == Qt::EditRole) {
		int idx = index.row();
		if (idx >= ScriptManager::instance()->scriptCount())
			return false;

		if (value.toString().isEmpty())
			return false;

		ScriptManager::instance()->scripts()[idx].name = value.toString();
		return true;
	}

	return false;
}

Qt::ItemFlags ScriptListModel::flags(const QModelIndex& index) const
{
	return Qt::ItemIsEnabled | Qt::ItemIsEditable | Qt::ItemIsSelectable;
}

QModelIndex ScriptListModel::append()
{

	beginResetModel();
	ScriptManager::instance()->append("New Script", QString(""));
	endResetModel();

	return index(ScriptManager::instance()->scriptCount() - 1);
}

void ScriptListModel::remove(const QModelIndex& index)
{
	int row = index.row();
	if (row >= ScriptManager::instance()->scriptCount())
		return;
	beginResetModel();
	ScriptManager::instance()->scripts().removeAt(row);
	endResetModel();
}
