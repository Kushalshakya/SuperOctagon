#include "mainwindow.h"
#include <QApplication>

int xCoordinate = 0;
int yCoordinate = 0;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.setFixedSize(800,500);
    w.show();
    return a.exec();
}
