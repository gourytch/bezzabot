#include <QtGui/QApplication>

#include <QDir>
#include <QDebug>
#include <QString>
#include "appwindow.h"
#include "tools/tools.h"
#include "tools/logger.h"

//#include <QTextCodec>

void dbgout() {
    qDebug("1.русские буквы в char*");
    qDebug("2.русские буквы в QString");
    qDebug(u8("3.русские буквы в u8"));
    qDebug(qApp->tr("4.русские буквы в tr"));
    qDebug(QString("5.arg=%1").arg(u8("русские буквы в QString::arg(QString)")));
    qDebug() << "6.русские буквы в char*";
    qDebug() << "7.русские буквы в QString";
    qDebug() << u8("8.русские буквы в u8");
    qDebug() << qApp->tr("9.русские буквы в tr");
    qDebug() << QString("10.arg=%1").arg(u8("русские буквы в QString::arg(QString)"));
}

int main (int argc, char ** argv) {
    QApplication app(argc, argv);

//    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
//    QTextCodec::setCodecForCStrings(codec);
//    QTextCodec::setCodecForTr(codec);
//    QTextCodec::setCodecForLocale(codec);

    dbgout();
    Config::global();
    dbgout();
    Logger::global();
    dbgout();
/*
    qDebug("initialize window");
    AppWindow w;
    w.show();
    qDebug("execute application");
    return app.exec();
*/
    return 0;
}
