#include <QtWidgets>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //запуск MainWindow
    MainWindow w;
    w.show();

    return a.exec();
}
