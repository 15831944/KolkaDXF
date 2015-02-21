#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    //QTextCodec::setCodecForTr (QTextCodec::codecForName ("UTF-8"));
    //QTextCodec::setCodecForCStrings (QTextCodec::codecForName ("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1250"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName ("indows-1250"));

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
