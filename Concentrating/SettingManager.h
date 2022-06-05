#pragma once
#include <qmap.h>
#include <qvariant.h>
#include <qstring.h>
#include <qurl.h>
#include "QVariantTreeNode.h"

class SettingManager
{
	SettingManager() {}
public:
	static SettingManager* instance(){ return _instance; }

	QVariant operator [] (const QString& key);
	QVariant value(const QString& key, const QVariant& defaultValue = QVariant());
	void setValue(const QString& key, const QVariant value);

	inline QVariantMap data()const { return _data->data().toMap(); }
	void setData(const QVariantMap& data);

	void remove(const QString& key);
	bool exist(const QString& key);

	bool load();
	bool save();

	inline int count()const { return _data->childrenCount(); }

	inline QStringList keys()const { return _data->mapChildrenKeys(); }

	inline QUrl path()const {return _path;};
	inline void setPath(const QUrl& path) { _path = path; }

private:
	QVariantTreeNode* findNode(const QString& path,bool create) const;

private:
	QUrl _path;
	QVariantTreeNode* _data = nullptr;

	static SettingManager* _instance;
};

