#include <QtGui/QApplication>
#include <QTextCodec>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8"));
    QTextCodec::setCodecForCStrings(QTextCodec::codecForName("UTF-8"));

    QCoreApplication::setOrganizationName("NRNU MEPhI");
    QCoreApplication::setApplicationName("CAPSYS");

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
