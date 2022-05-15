#pragma once

#include <QComboBox>
#include <qvariant.h>
#include <qvector.h>

class QVariantTypeComboBox : public QComboBox
{
	Q_OBJECT

public:
	QVariantTypeComboBox(QWidget *parent);
	QVariantTypeComboBox(QVector<QVariant::Type> types, QWidget* parent);
	~QVariantTypeComboBox();

	QVariant::Type type()const;
	void setType(QVariant::Type type);

	void setVaildTypes(QVector<QVariant::Type> types);
	QVector<QVariant::Type> vaildTypes()const { return _vaildTypes; }

	void addType(QVariant::Type type);

private:
	QVector<QVariant::Type> _vaildTypes;
};
