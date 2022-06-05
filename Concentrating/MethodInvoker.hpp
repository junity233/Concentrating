#pragma once

#include <QObject>
#include <functional>

class MethodInvoker 
	: public QObject
{
	Q_OBJECT

public:
	MethodInvoker(QObject* parent = nullptr)
		:QObject(parent) {}
signals:
	void invoke();

private:
	std::function<void()> _func;
};
