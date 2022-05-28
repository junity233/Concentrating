#include "QVariantTreeNode.h"
#include <qdatetime.h>
#include <qurl.h>

QVariantTreeNode::QVariantTreeNode(const QVariant& data, QVariantTreeNode* parent):
	QObject(parent),
	_var(),
	_children(),
	_keys()
{
	_parent = parent;
	setData(data);
}

QVariantTreeNode::QVariantTreeNode(Type type, QVariantTreeNode* parent) :
	QObject(parent),
	_var(),
	_children(),
	_keys()
{
	_type = type;
	_parent = parent;
	_changed = true;
}

QVariantTreeNode::~QVariantTreeNode()
{
	if(_type!=Variant)
		for (auto i : _children)
			delete i;
}

bool QVariantTreeNode::hasChild() const
{
	return _type != Variant;
}

int QVariantTreeNode::addListChild(QVariantTreeNode* child)
{
	if (_type == List) {
		_children.append(child);
		return _children.size();
	}

	return -1;
}

int QVariantTreeNode::addMapChild(const QString& key, QVariantTreeNode* child)
{
	if (_type == Map) {
		if (_keys.contains(key)) {
			int idx = _keys.indexOf(key);
			delete _children[idx];
			_children[idx] = child;
		}

		setDataChanged();
		_children.append(child);
		_keys.append(key);
		return _children.size();
	}

	return -1;
}

QVariantTreeNode* QVariantTreeNode::child(int index) const
{
	if (index < 0 || index >= childrenCount())
		return Q_NULLPTR;

	if (_type != Variant)
		return _children[index];

	return Q_NULLPTR;
}

QVector<QVariantTreeNode*> QVariantTreeNode::children() const
{
	if (_type != Variant)
		return _children;
	else return QVector<QVariantTreeNode*>();
}

QStringList QVariantTreeNode::mapChildrenKeys() const
{
	if (_type == Map)
		return _keys.toList();

	return QStringList();
}

QString QVariantTreeNode::mapChildKey(int index) const
{
	return _keys[index];
}

int QVariantTreeNode::childrenCount() const
{
	switch (_type) {
	case Map:
	case List:
		return _children.count();
	case Variant:
		return 0;
	}

	return 0;
}

bool QVariantTreeNode::mapRenameChild(int idx, const QString& name)
{
	if (_type != Map)
		return false;

	_keys[idx] = name;
	setDataChanged();

	return true;
}

int QVariantTreeNode::childrenNumber() const
{
	auto p = parent();

	if (p->_type == List)
		return p->_children.indexOf(const_cast<QVariantTreeNode*>(this));

	return 0;
}

void QVariantTreeNode::setData(const QVariant& data)
{
	switch (data.type()) {
	case QVariant::Map:
		fromMap(data.toMap());
		break;
	case QVariant::List:
		fromList(data.toList());
		break;
	default:
		_type = Variant;
		_var = data;
	}

	setDataChanged();
	_buf = data;
	_changed = false;
}

QVariant QVariantTreeNode::data() const
{
	if (!_changed)
		return _buf;

	if (_type == Variant) {
		_buf = _var;
		_changed = false;
		return _var;
	}
	else if (_type == Map)
		return toMap();
	else return toList();
}

bool QVariantTreeNode::remove(int pos, int count)
{
	if (_type == Variant)
		return false;
	for (int i = pos; i < pos + count; i++) {
		QVariantTreeNode* node = child(i);

		if (node) {
			delete node;
			
			_children.remove(i);
			if (_type == Map)
				_keys.remove(i);
		}
	}
	setDataChanged();

	return true;
}

bool QVariantTreeNode::insert(int pos, int count)
{
	if (_type == Variant)
		return false;


	setDataChanged();

	if (_type == List) {
		_children.insert(pos, count, new QVariantTreeNode(0, this));
		return true;
	}

	//map

	int idx = 1;
	for (int i = 0; i < count; i++)
		idx = mapInsertNewItem(idx);

	return true;
}

bool QVariantTreeNode::remove(QVariantTreeNode* child)
{
	int idx = _children.indexOf(child);

	if (idx < 0)
		return false;

	delete _children[idx];
	_children.remove(idx);
	_keys.remove(idx);
	setDataChanged();

	return true;
}

void QVariantTreeNode::clear()
{
	if (_type == Variant)
	{
		_var = QVariant();
		return;
	}

	for (auto i : _children)
		delete i;

	_children.clear();
	_keys.clear();
	setDataChanged();
}

void QVariantTreeNode::mapRemove(const QString& name)
{
	if (!_type == Map)
		return;

	int idx = _keys.indexOf(name);
	if (idx == -1)
		return;

	delete _children[idx];

	_keys.remove(idx);
	_children.remove(idx);
	setDataChanged();
}

void QVariantTreeNode::mapInsert(const QString& name, QVariantTreeNode* child)
{
	if (!_type == Map)
		return;

	_keys.append(name);
	_children.append(child);
	setDataChanged();
}

void QVariantTreeNode::changeType(QVariant::Type type)
{
	switch (type) {
	case QVariant::Map:
		if (_type == Map)
			return;
		if (_type == List) {
			_type = Map;
			_keys.clear();

			for (int i = 0; i < _children.size(); i++)
				_keys.append(QString::number(i + 1));
		}
		else fromMap(QVariantMap());
		break;
	case QVariant::List:
		if (_type == List)
			return;
		if (_type == Map) {
			_type = List;
			_keys.clear();
		}
		else fromList({ _var });
		break;
	default:
		if (_type != Variant) {
			clear();
			_type = Variant;
		}

		else if (_var.type() == type)
			return;

		switch (type) {
		case QVariant::Bool:
			_var = false;
			break;
		case QVariant::Int:
			_var = 0;
			break;
		case QVariant::String:
			_var = QString();
			break;
		case QVariant::Time:
			_var = QTime();
			break;
		case QVariant::DateTime:
			_var = QDateTime();
			break;
		case QVariant::Date:
			_var = QDate();
			break;
		case QVariant::Double:
			_var = 0.0;
			break;
		case QVariant::Url:
			_var = QUrl();
			break;
		default:
			_var = QVariant();
		}
		
	}
	setDataChanged();
}

QVariant::Type QVariantTreeNode::toQVariantType() const
{
	if (_type == List)
		return QVariant::List;
	if (_type == Map)
		return QVariant::Map;

	return _var.type();
}

QVariantTreeNode* QVariantTreeNode::copy(QVariantTreeNode* parent) const
{
	QVariantTreeNode* res = new QVariantTreeNode(data(), parent);
	return res;
}

void QVariantTreeNode::fromMap(const QVariantMap& map)
{
	
	clear();
	_type = Map;
	
	for (auto i : map.keys()) {
		addMapChild(i, new QVariantTreeNode(map[i], this));
	}
	setDataChanged();
}

void QVariantTreeNode::fromList(const QVariantList& list)
{
	clear();
	_type = List;

	for (auto i : list) {
		addListChild(new QVariantTreeNode(i, this));
	}

	setDataChanged();
}

QVariantMap QVariantTreeNode::toMap() const
{
	QVariantMap res;
	if (_type != Map)
		return QVariantMap();

	for (int i = 0; i < _children.size(); i++) {
		res.insert(_keys[i], _children[i]->data());
	}

	_buf = res;
	_changed = false;
	return res;
}

QVariantList QVariantTreeNode::toList() const
{
	QVariantList res;
	if (_type != List)
		return QVariantList();

	for (auto i : _children) {
		res.append(i->data());
	}

	_buf = res;
	_changed = false;
	return res;
}

int QVariantTreeNode::mapInsertNewItem(int idx)
{
	while (_keys.contains(tr("New Item(%1)").arg(idx)))
		idx++;

	mapInsert(tr("New Item(%1)").arg(idx), new QVariantTreeNode(QString(), this));
	return idx;
}

void QVariantTreeNode::setDataChanged()
{
	_changed = true;
	QVariantTreeNode* p = parent();

	if (p == Q_NULLPTR)
		return;

	p->setDataChanged();
}
