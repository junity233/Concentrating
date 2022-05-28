#pragma once

#include <QObject>

class MethodInvoker 
	: public QObject
{
	Q_OBJECT

public:
	MethodInvoker(QObject* parent = nullptr)
		:QObject(parent) {}

signals:
	void invoke();
};
