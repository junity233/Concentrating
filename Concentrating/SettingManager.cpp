#include "SettingManager.h"
#include <qfile.h>
#include <qiodevice.h>
#include <qjsondocument.h>
#include <qjsonobject.h>
#include <qdatastream.h>
#include <qurl.h>

SettingManager* SettingManager::_instance = new SettingManager;

QVariant SettingManager::operator[](const QString& key)
{
	return findNode(key, true)->data();
}

QVariant SettingManager::value(const QString& key, const QVariant& defaultValue)
{
	QVariantTreeNode* node = findNode(key, true);
	if (node == nullptr)
		return QVariant();

	if (!node->data().isValid())
		node->setData(defaultValue);

	return node->data();
}

void SettingManager::setValue(const QString& key, const QVariant value)
{
	QVariantTreeNode* node = findNode(key, true);
	if (node == nullptr)
		return;
	node->setData(value);
}

void SettingManager::setData(const QVariantMap& data)
{
	_data->setData(data);
}

void SettingManager::remove(const QString& key)
{
	QVariantTreeNode* node = findNode(key, false);
	if (node == nullptr)
		return;

	node->parent()->remove(node);

}

bool SettingManager::exist(const QString& key)
{
	return findNode(key, false) != nullptr;
}

bool SettingManager::load()
{
	QFile f(_path.toLocalFile());
	f.open(QIODevice::ReadOnly);

	if (!f.isOpen()) {
		if (!_data)
			_data = new QVariantTreeNode(QVariantTreeNode::Map);
		return false;
	}

	QDataStream in(&f);
	QVariant t;
	in >> t;

	if(_data)
		delete _data;
	_data = new QVariantTreeNode(t.toMap(), nullptr);

	return true;
}

bool SettingManager::save()
{
	QFile f(_path.toLocalFile());
	f.open(QIODevice::WriteOnly);

	if (!f.isOpen())
		return false;

	QDataStream out(&f);

	out << _data->data();

	f.close();

	return true;
}

QVariantTreeNode* SettingManager::findNode(const QString& path,bool create) const
{
	QStringList list = path.split(".");
	if (list.isEmpty())
		return nullptr;

	QVariantTreeNode* now = _data;

	if (now == nullptr)
		return nullptr;

	for (int i = 0; i < list.size(); i++) {
		QString key = list[i];
		if (now->type() == QVariantTreeNode::Map) {
			int idx = now->mapChildrenKeys().indexOf(key);

			if (idx >= 0) {
				now = now->child(idx);
			}
			else if (create) {
				QVariantTreeNode* t = new QVariantTreeNode(QVariantTreeNode::Map, now);
				now->addMapChild(key, t);
				now = t;
			}
			else return nullptr;
		}
		else return nullptr;
	}

	return now;
}
