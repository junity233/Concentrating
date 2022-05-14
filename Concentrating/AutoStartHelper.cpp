#include "AutoStartHelper.h"
#include <qapplication.h>
#include <qsettings.h>
#include <qstring.h>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>
#include <qcoreapplication.h>

QString AutoStartHelper::regPath = R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run)";
QString AutoStartHelper::key = "Concerntrating";

void AutoStartHelper::setAutoStartEnable(bool state)
{
    QSettings setting(regPath, QSettings::NativeFormat);
    QString path = QCoreApplication::applicationFilePath();

    path.replace("/", "\\");

    if (state)
        setting.setValue(key, QString("\"%1\" --autostart").arg(path));
    else {
        if (setting.contains(key))
            setting.remove(key);
    }
}

bool AutoStartHelper::isAutoStartEnable()
{
    QSettings setting(regPath, QSettings::NativeFormat);

    return setting.contains(key);
}

bool AutoStartHelper::isApplicationAutoStart()
{
    QCommandLineParser parser;
    QCommandLineOption autoStartOption("autostart");

    parser.addOption(autoStartOption);

    parser.process(QCoreApplication::instance()->arguments());

    return parser.isSet(autoStartOption);
}
