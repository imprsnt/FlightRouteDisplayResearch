#include "mainwindow.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication prog(argc, argv);
    MainWindow mainWindow;
    mainWindow.show();

    return prog.exec();
}
