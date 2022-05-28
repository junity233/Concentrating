#include "QVariantModel.h"
#include <qobject.h>

QVariantModel::QVariantModel(QObject* parent)
    :QAbstractItemModel(parent)
{
    _root = Q_NULLPTR;
}

QVariantModel::QVariantModel(const QVariant& var,QObject* parent)
    : QAbstractItemModel(parent)
{
    _root = new QVariantTreeNode(var);
}

QVariantTreeNode* QVariantModel::getItem(const QModelIndex& index) const
{
    if (index.isValid()) {
        QVariantTreeNode* item = static_cast<QVariantTreeNode*>(index.internalPointer());
        if (item)
            return item;
        return Q_NULLPTR;
    }
    return _root;
}

QVariantModel::~QVariantModel()
{
    delete _root;
}

QVariant QVariantModel::data(const QModelIndex& index, int role) const
{
    auto item = getItem(index);

    switch (role)
    {
    case Qt::DisplayRole:
    case Qt::EditRole:
        if (index.column() == 0) {
            auto parentIdx = parent(index);
            auto parentPtr = getItem(parentIdx);

            if (parentPtr->type() == QVariantTreeNode::List)
                return index.row();
            else return parentPtr->mapChildKey(index.row());
        }
        else if(index.column()==1){
            if (item->type() == QVariantTreeNode::List)
                return tr("[List]");
            else if (item->type() == QVariantTreeNode::Map)
                return tr("[Map]");
            else return item->data();
        }
        else  if (index.column() == 2) {
            if (role == Qt::DisplayRole)
                return QVariant::typeToName(item->toQVariantType());
            else if(role==Qt::EditRole)
                return (int)item->toQVariantType();
        }
    default:
        return QVariant();
    }
}

QVariant QVariantModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole) {
        if (orientation == Qt::Horizontal) {
            if (section == 0)
                return tr("Item");
            else if (section == 1)
                return tr("Data");
            else if (section == 2)
                return tr("Type");
        }
    }

    return QVariant();
}

QModelIndex QVariantModel::index(int row, int column, const QModelIndex& parent) const
{
    if (parent.isValid() && parent.column() != 0)
        return QModelIndex();

    auto item = getItem(parent);

    if (item) {

        auto child = item->child(row);

        if (child)
            return createIndex(row, column, child);
    }

    return QModelIndex();
}

QModelIndex QVariantModel::parent(const QModelIndex& index) const
{
    if (!index.isValid())
        return QModelIndex();
    auto item = getItem(index);

    if (!item)
        return QModelIndex();

    auto p = item->parent();

    if (p == _root || !p)
        return QModelIndex();

    return createIndex(p->childrenNumber(), 0, p);
}

int QVariantModel::rowCount(const QModelIndex& parent) const
{
    if (parent.isValid()) {
        auto p = getItem(parent);
        return p->childrenCount();
    }
    else {
        if (_root)
            return _root->childrenCount();
    }

    return 0;
}

int QVariantModel::columnCount(const QModelIndex& parent) const
{
    return 3;
}

Qt::ItemFlags QVariantModel::flags(const QModelIndex& index) const
{
    if (index.isValid()) {
        QVariantTreeNode* item = getItem(index);

        if (index.column() == 0) {
            if(item->parent()->type()==QVariantTreeNode::List)
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
        else if (index.column() == 1) {
            if (item->type() != QVariantTreeNode::Variant)
                return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
        }
    }

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsEditable;
}

bool QVariantModel::setData(const QModelIndex& index, const QVariant& value, int role)
{
    if (!index.isValid())
        return false;
    if (role == Qt::EditRole) {
        QVariantTreeNode* item = getItem(index);

        if (item) {
            if (index.column() == 0) {
                QVariantTreeNode* p = item->parent();
                int row = index.row();
                if (!p)
                    return false;

                if (p->type() == QVariantTreeNode::List)
                    return false;
                else {
                    if (value.type() != QVariant::String)
                        return false;

                    bool res = p->mapRenameChild(index.row(), value.toString());

                    return true;
                }

            }
            else if (index.column() == 1) {
                item->setData(value);

                return true;
            }
            else if (index.column() == 2) {
                auto type = (QVariant::Type)value.toInt();
                if (type != QVariant::Map && type != QVariant::List && item->childrenCount() > 0) {
                    beginRemoveRows(index, 0, item->childrenCount() - 1);
                    item->changeType(type);
                    endRemoveRows();
                }
                else {
                    item->changeType(type);
                }
                return true;
            }

        }
    }

    return false;
}

void QVariantModel::setVariant(const QVariant& var)
{
    beginResetModel();
    delete _root;

    _root = new QVariantTreeNode(var);

    endResetModel();
}

QVariant QVariantModel::variant() const
{
    if (_root)
        return _root->data();
    return QVariant();
}

bool QVariantModel::removeRows(int row, int count, const QModelIndex& parent)
{
    QVariantTreeNode* item = getItem(parent);
    //QVariantTreeNode*
    if (!item)
        return false;

    beginRemoveRows(this->parent(parent), row, row + count - 1);
    bool res = item->parent()->removeRows(row, count);
    endRemoveRows();

    return res;
}

bool QVariantModel::insertRows(int row, int count, const QModelIndex& parent)
{
    QVariantTreeNode* item = getItem(parent);
    if (!item)
        return false;

    beginInsertRows(parent, row, row + count - 1);
    bool res = item->insertRows(row, count);
    endInsertRows();

    return res;
}
