#include "gui/mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("DarkPassenger");
    QCoreApplication::setApplicationName("osfsc");

    MainWindow w;
    w.show();
    return a.exec();
}
