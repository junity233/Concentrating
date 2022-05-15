#pragma once
#include <qvariant.h>
#include <qvector.h>
#include <qobject.h>

class QVariantTreeNode
	:public QObject
{
	Q_OBJECT
public:
	enum Type {
		List,
		Map,
		Variant
	};

	explicit QVariantTreeNode(const QVariant& var, QVariantTreeNode* parent=Q_NULLPTR);
	~QVariantTreeNode();

	Type type()const { return _type; }

	bool hasChild()const;

	void addListChild(QVariantTreeNode* child);
	void addMapChild(const QString& key, QVariantTreeNode* child);

	QVariantTreeNode* child(int index) const;
	QVector<QVariantTreeNode*> children()const;
	QStringList mapChildrenKeys()const;
	QString mapChildKey(int index)const;
	int childrenCount()const;

	bool mapRenameChild(int idx, const QString& name);

	QVariantTreeNode* parent()const {return _parent; }
	int childrenNumber()const;

	void setData(const QVariant& data);
	QVariant data()const;

	bool removeRows(int pos, int count);
	bool insertRows(int pos, int count);

	void clear();

	void mapRemove(const QString& name);
	void mapInsert(const QString& name, QVariantTreeNode* child);


private:
	void fromMap(const QVariantMap& map);
	void fromList(const QVariantList& list);

	QVariantMap toMap()const;
	QVariantList toList()const;

	int mapInsertNewItem(int idx = 1);


private:
	QVariantTreeNode* _parent;
	Type _type;

	QVariant _var;
	QVector<QVariantTreeNode*> _children;
	QVector<QString> _keys;

	mutable QVariant _data;
	mutable bool _changed;
protected:
	void setDataChanged();
};

