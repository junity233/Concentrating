#include "ScriptManager.h"
#include <qfile.h>
#include <qiodevice.h>
#include <qjsondocument.h>
#include <Qjsonarray.h>
#include <qjsonobject.h>
#include <qvariant.h>

ScriptManager* ScriptManager::_instance = new ScriptManager;

void ScriptManager::append(const QString& name, const QString& script)
{
	_scripts.append({ name,script });
}

void ScriptManager::remove(int index)
{
	if (_scripts.size() >= index)
		_scripts.removeAt(index);
}

bool ScriptManager::load(const QString& file)
{
	QFile f(file);
	f.open(QIODevice::ReadOnly);

	if (!f.isOpen())
		return false;

	QJsonDocument docu = QJsonDocument::fromJson(f.readAll());
	f.close();

	if (docu.isNull())
		return false;

	auto data = docu.array();

	for (auto i : data) {
		QJsonObject item = i.toObject();

		Script script;
		script.name = item.value("name").toString();
		script.code = item.value("code").toString();

		_scripts.append(script);
	}

	return true;
}

bool ScriptManager::save(const QString& file)
{
	QFile f(file);
	f.open(QIODevice::WriteOnly);

	if (!f.isOpen())
		return false;

	QJsonDocument docu;
	QVariantList varList;

	for (auto i : _scripts) {
		QJsonObject item;
		item["name"] = i.name;
		item["code"] = i.code;
		varList.append(item);
	}

	docu.setArray(QJsonArray::fromVariantList(varList));

	f.write(docu.toJson());
	f.close();

	return true;
}
