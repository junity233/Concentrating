#include "LogPage.h"
#include <qdatetime.h>

LogPage::LogPage(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
}

LogPage::~LogPage()
{
}

void LogPage::log(const QString& msg, Role role)
{
	QString roleName;

	switch (role) {
	case Script:roleName = "Script"; break;
	case System:roleName = "System"; break;
	case Lua:roleName = "Lua"; break;
	default:roleName = "Unknown Role";
	}

	ui.logger->append(QString("<font color=\"green\">[%1]</font>  <font color=\"red\">%2</font> : %3\n")
		.arg(QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss"))
		.arg(roleName)
		.arg(msg)
	);
}
