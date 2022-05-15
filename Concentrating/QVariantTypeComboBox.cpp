#include "QVariantTypeComboBox.h"

QVariantTypeComboBox::QVariantTypeComboBox(QWidget *parent)
	: QComboBox(parent)
{

}

QVariantTypeComboBox::~QVariantTypeComboBox()
{
}

QVariant::Type QVariantTypeComboBox::type() const
{
	return _vaildTypes[currentIndex()];
}

void QVariantTypeComboBox::setType(QVariant::Type type)
{
	int idx = _vaildTypes.indexOf(type);
	if (idx != -1)
		setCurrentIndex(idx);
}

void QVariantTypeComboBox::setVaildTypes(QVector<QVariant::Type> types)
{
	_vaildTypes = types;
	clear();

	for (auto i : types) {
		addItem(QVariant::typeToName(i));
	}
}

void QVariantTypeComboBox::addType(QVariant::Type type)
{
	_vaildTypes.append(type);
	addItem(QVariant::typeToName(type));
}
