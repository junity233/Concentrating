#include "SettingManager.h"
#include <qfile.h>
#include <qiodevice.h>
#include <qjsondocument.h>
#include <qjsonobject.h>

SettingManager* SettingManager::_instance = new SettingManager;

QVariant& SettingManager::operator[](const QString& key)
{
	if (_data.contains(key))
		return _data[key];
	_data.insert(key, QVariant());
	return _data[key];
}

QVariant& SettingManager::value(const QString& key, const QVariant defaultValue)
{
	if (_data.contains(key)) {
		return _data[key];
	}
	_data.insert(key, defaultValue);
	return _data[key];
}

void SettingManager::setValue(const QString& key, const QVariant value)
{
	_data[key] = value;
}

void SettingManager::setData(const QVariantMap& data)
{
	_data = data;
}

bool SettingManager::load(const QString& file)
{
	QFile f(file);
	f.open(QIODevice::ReadOnly);

	if (!f.isOpen())
		return false;

	QJsonDocument docu = QJsonDocument::fromJson(f.readAll());
	f.close();

	if (docu.isNull())
		return false;

	_data = docu.object().toVariantMap();

	return true;
}

bool SettingManager::save(const QString& file)
{
	QFile f(file);
	f.open(QIODevice::WriteOnly);

	if (!f.isOpen())
		return false;

	QJsonDocument docu;
	docu.setObject(QJsonObject::fromVariantMap(_data));

	f.write(docu.toJson());
	f.close();

	return true;
}