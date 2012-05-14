#include <QtGui/QApplication>
#include<QIODevice>
#include <QFile>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QRegExp>
#include "appwindow.h"
#include "tools/tools.h"
#include "tools/logger.h"
#include "tools/activityhours.h"
#include "tools/tarball.h"
#include "tools/varmap.h"
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

void parseLoot(const QString& s) {
    QRegExp rx(u8("<span\\s*[^>]*>([^<]+)</span><span\\s*[^>]*>получил\\s*(.*)"));
    if (rx.indexIn(s) == -1) {
        qCritical("parseLoot: regexp does not match");
        return;
    }

    QString name = rx.cap(1).trimmed();
    QString loot = rx.cap(2).trimmed().replace("&nbsp;", " ");
    qDebug("winner : " + name);
    QRegExp rx_gold(u8("^<span\\s+class=\"price_num\">\\s*([0123456789.]+)\\s*</span>"
                "\\s*<b [^>]+title=\"(Золото)\">\\s*</b>\\s*(.*)$"));
    QRegExp rx_res(u8("^([0123456789.]+)\\s*<b [^>]+title=.([^>]+).>"
                      "\\s*</b>\\s*(.*)$"));


    int amount;
    QString title;

    while (loot.length() > 0) {
        loot = loot.trimmed();
        if (rx_gold.indexIn(loot) != -1) {
            amount = dottedInt(rx_gold.cap(1));
            title = rx_gold.cap(2).trimmed();
            loot = rx_gold.cap(3);
            qDebug(u8("%1:%2").arg(title).arg(amount));
            continue;
        }

        if (rx_res.indexIn(loot) != -1) {
            amount = dottedInt(rx_res.cap(1));
            title = rx_res.cap(2).trimmed();
            loot = rx_res.cap(3);
            qDebug(u8("%1:%2").arg(title).arg(amount));
            continue;
        }

        if (loot == "</span></td>") {
            break;
        }

        qCritical(u8("строка не подходит: {%1}").arg(loot));
        break;
    }
}

void testLootParser() {
    QFile f("loot.txt");
    if (!f.open(QIODevice::ReadOnly)) {
        qFatal("loot.txt not opened");
        return;
    }
    QString s;
    while (!f.atEnd()) {
        s = f.readLine();
        qDebug(u8("test %1").arg(s));
        parseLoot(s);
    }
    f.close();
}

void testTarball() {
    Tarball tarball;
    if (!tarball.open("tar_test")) {
        qFatal("tarball not opened");
        return;
    }
    tarball.add("first.txt", QByteArray("{this is my first datafile}"));
    tarball.add("second.txt", QByteArray("{this is my second datafile}"));
    tarball.close();
}

void testGzip() {
    QByteArray src("{*0123456789ABCDEF*}");
    ::save("testGzip_raw", src);
    ::save("testGzip_compressed", qCompress(src, 9));
    ::save("testGzip_gziped", gzipFile(src));
}

void testVarMap() {
    {
        qDebug("initialize");
        VarMap vars("test.varmap");
        vars.clear();
        vars.set("simple", "foo bar");
        vars.set("equal_sign", "e = mc²");
        vars.set("/comp/lex/value", "раз\nдва\nтри\nчетыре\nпять");
        qDebug("read 1");
        qDebug(u8("simple = {%1}").arg(vars.get("simple").toString()));
        qDebug(u8("equal_sign = {%1}").arg(vars.get("equal_sign").toString()));
        qDebug(u8("/comp/lex/value = {%1}").arg(vars.get("/comp/lex/value").toString()));
    }
    {
        qDebug("reinitialize");
        VarMap vars("test.varmap");
        qDebug(u8("simple = {%1}").arg(vars.get("simple").toString()));
        qDebug(u8("equal_sign = {%1}").arg(vars.get("equal_sign").toString()));
        qDebug(u8("/comp/lex/value = {%1}").arg(vars.get("/comp/lex/value").toString()));
    }

}


int main (int argc, char ** argv) {
    QApplication app(argc, argv);
    Config::global();
    Logger::global();
//    testVarMap();
//    return 0;
    //////////////////////////////////////
    //////////////////////////////////////
    qDebug("initialize window");
    AppWindow w;
    w.show();
    qDebug("execute application");
    return app.exec();
    return 0;
}
