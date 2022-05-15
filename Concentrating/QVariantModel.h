#pragma once

#include <qabstractitemmodel.h>
#include "QVariantTreeNode.h"

class QVariantModel :
    public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit QVariantModel(QObject* parent = Q_NULLPTR);
    explicit QVariantModel(const QVariant& var, QObject* parent = Q_NULLPTR);
    ~QVariantModel();

    QVariant data(const QModelIndex& index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation,
        int role = Qt::DisplayRole) const override;

    QModelIndex index(int row, int column,
        const QModelIndex& parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex& index) const override;

    int rowCount(const QModelIndex& parent = QModelIndex()) const override;
    int columnCount(const QModelIndex& parent = QModelIndex()) const override;

    //Read-only tree models only need to provide the above functions. The following public functions provide support for editing and resizing:

    Qt::ItemFlags flags(const QModelIndex& index) const override;
    bool setData(const QModelIndex& index, const QVariant& value,
        int role = Qt::EditRole) override;

    void setVariant(const QVariant& var);
    QVariant variant()const;

    bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex())override;
    bool insertRows(int row, int count, const QModelIndex& parent = QModelIndex())override;

private:
    QVariantTreeNode* getItem(const QModelIndex& index)const;
private:
    QVariantTreeNode* _root;
};

