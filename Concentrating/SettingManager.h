#pragma once
#include <qmap.h>
#include <qvariant.h>
#include <qstring.h>

class SettingManager
{
	SettingManager() {}
public:
	static SettingManager* instance(){ return _instance; }

	QVariant& operator [] (const QString& key);
	QVariant& value(const QString& key, const QVariant defaultValue = QVariant());
	void setValue(const QString& key, const QVariant value);

	QVariantMap data()const { return _data; }
	void setData(const QVariantMap& data);

	bool load(const QString& file);
	bool save(const QString& file);

	int count()const { return _data.size(); }

	QStringList keys()const { return _data.keys(); }

private:
	QVariantMap _data;

	static SettingManager* _instance;
};

