#include <QtGui/QApplication>

#include <QDir>
#include <QDebug>
#include <QString>
#include <QRegExp>
#include "appwindow.h"
#include "tools/tools.h"
#include "tools/logger.h"
#include "tools/activityhours.h"
//#include <QTextCodec>

void ahtest(const char *s) {
    ActivityHours ah;
    ah.assign(u8(s));
    qDebug("SRC: {%s}", s);
    qDebug(ah.toString());
    qDebug(ah.timeline());
    qDebug("seg_length from 5 = %d", ah.seg_length(5));
}

void ahtest() {
    ahtest("");
    ahtest("0-23");
    ahtest("7-12");
    ahtest("1 3 5 7");
    ahtest("1 3-5 7");
    ahtest("21-7");
    ahtest("21-20");
}

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
    QRegExp rx("/([^/]+)\\.jpg");
    rx.indexIn("background:url(http://i.botva.ru/images/monster/NY/m1_gb.jpg) no-repeat;width:190px;height:240px;");
    qDebug("{" + rx.cap(1) + "}");
    return 0;

    dbgout();
    Config::global();
    dbgout();
    Logger::global();
    dbgout();
/**/
    qDebug("initialize window");
    AppWindow w;
    w.show();
    qDebug("execute application");
    return app.exec();
/**/
    return 0;
}
