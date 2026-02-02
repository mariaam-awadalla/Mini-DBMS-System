#include <QApplication>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);//initialize Qt framework --> GUI , EVENTS

    MainWindow w;
    w.show();

    return app.exec();
}
