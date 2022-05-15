#include "DesktopHelper.h"
#include <Windows.h>

DesktopHelper::Desktop DesktopHelper::GetCurrentDesktop()
{
    return OpenInputDesktop(0, false, READ_CONTROL);
}

DesktopHelper::Desktop DesktopHelper::GetThreadDesktop()
{
    return ::GetThreadDesktop(GetCurrentProcessId());
}

DesktopHelper::Desktop::Desktop(HDESK__* hDesk)
{
    _hDesk = hDesk;
}

QString DesktopHelper::Desktop::name()
{
    if (_hDesk)
    {
        DWORD dwNeeded = 0;
        if (!::GetUserObjectInformation(_hDesk, UOI_NAME, NULL, 0, &dwNeeded) && dwNeeded)
        {
            dwNeeded += sizeof(TCHAR);
            LPTSTR buf = new wchar_t[dwNeeded];

            if (::GetUserObjectInformation(_hDesk, UOI_NAME, buf, dwNeeded, &dwNeeded))
            {
                QString res = QString::fromWCharArray(buf);
                delete[] buf;
                return res;
            }
        }
    }
    return QString();
}

bool DesktopHelper::Desktop::switchTo()
{
    return ::SwitchDesktop(_hDesk);
}
