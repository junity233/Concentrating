#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <qtranslator.h>
#include <qdir.h>

#pragma data_seg("shared")

bool started = false;

#pragma data_seg()
#pragma comment(linker, "/SECTION:shared,RWS")  //设置为共享内存


int main(int argc, char *argv[])
{
    if (started)
        return 0;

    started = true;
    QApplication a(argc, argv);

    QDir::setCurrent(QApplication::applicationDirPath());

    QTranslator t;
    t.load("zh");

    QApplication::installTranslator(&t);

    MainWindow w;

    a.setQuitOnLastWindowClosed(false);

    return a.exec();
}
