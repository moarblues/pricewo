#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator ruTrans;
    ruTrans.load("qtbase_ru",QDir::currentPath().append("/trans"));
    a.installTranslator(&ruTrans);

    mainWindow w;
    w.show();
    
    return a.exec();
}
