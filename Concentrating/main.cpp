#include "MainWindow.h"
#include <QtWidgets/QApplication>
#include <signal.h>
#include "AutoStartHelper.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;

    if(!AutoStartHelper::isApplicationAutoStart())
        w.show();
    return a.exec();
}
