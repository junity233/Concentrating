#pragma once
#include <qmap.h>
#include <qvariant.h>
#include <qstring.h>
#include "QVariantTreeNode.h"

class QVariantTreeNode;

class SettingManager
{
	SettingManager() {}
public:
	static SettingManager* instance(){ return _instance; }

	QVariant operator [] (const QString& key);
	QVariant value(const QString& key, const QVariant& defaultValue = QVariant());
	void setValue(const QString& key, const QVariant value);

	QVariantMap data()const { return _data->data().toMap(); }
	void setData(const QVariantMap& data);

	void remove(const QString& key);
	bool exist(const QString& key);

	bool load(const QString& file);
	bool save(const QString& file);

	int count()const { return _data->childrenCount(); }

	QStringList keys()const { return _data->mapChildrenKeys(); }

private:
	QVariantTreeNode* findNode(const QString& path,bool create) const;

private:
	QVariantTreeNode* _data = nullptr;

	static SettingManager* _instance;
};

