#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <signal.h>
#include <qtranslator.h>
#include "AutoStartHelper.h"

#pragma data_seg("shared")

bool started = false;

#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")  //����Ϊ�����ڴ�


int main(int argc, char *argv[])
{
    if (started)
        return 0;

    started = true;
    QApplication a(argc, argv);

    QTranslator t;
    t.load("zh");

    QApplication::installTranslator(&t);

    MainWindow w;

    a.setQuitOnLastWindowClosed(false);

    return a.exec();
}
