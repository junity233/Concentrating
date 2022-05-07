#include "AutoStartHelper.h"
#include <qapplication.h>
#include <qsettings.h>
#include <qstring.h>

QString AutoStartHelper::regPath = R"(HKEY_CURRENT_USER\Software\Microsoft\Windows\CurrentVersion\Run)";
QString AutoStartHelper::key = "Concerntrating";

void AutoStartHelper::setAutoStart(bool state)
{
    QSettings setting(regPath, QSettings::NativeFormat);
    QString path = QCoreApplication::applicationFilePath();

    path.replace("/", "\\");

    if (state)
        setting.setValue(key, path);
    else {
        if (setting.contains(key))
            setting.remove(key);
    }
}

bool AutoStartHelper::isAutoStart()
{
    QSettings setting(regPath, QSettings::NativeFormat);

    return setting.contains(key);
}
