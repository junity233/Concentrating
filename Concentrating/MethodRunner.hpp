#pragma once

#include <QRunnable>
#include <functional>

class MethodRunner 
	: public QRunnable
{
public:
	MethodRunner(std::function<void()> method)
		:QRunnable(),
		_method(method) 
	{}

	void run() override
	{
		_method();
	}

private:
	std::function<void()> _method;
};
