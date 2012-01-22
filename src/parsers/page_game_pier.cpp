#include <QRegExp>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <QDate>
#include <QTime>
#include "tools/tools.h"
#include "page_game_pier.h"

Page_Game_Pier::Page_Game_Pier (QWebElement& doc) :
    Page_Game (doc) {
    pagekind = page_Game_Pier;
    QString t = body.findFirst("CENTER").toPlainText();
    QRegExp rx (u8("получить улов в виде (\\d+)"));
    if (rx.indexIn(t) > -1) {
        raid_capacity = rx.cap(1).toInt();
    }
    QWebElementCollection c = body.findFirst("TABLE").findAll("TD");

    num_fishes  = -1;
    num_boats   = -1;
    num_ships   = -1;
    num_steamers= -1;

    for (int cnt = 0; cnt < c.count(); ++cnt) {
//        qDebug () << QString("TD[%1] ={%2}").arg(cnt).arg();
        rx = QRegExp(u8("(.*)Кол-во:\\s+(\\d+)"));
        t = c[cnt].toPlainText();
        if (rx.indexIn(t) != -1) {
            QString name = rx.cap(1).trimmed();
            int count = rx.cap(2).trimmed().toInt();
            if (name == u8("Пирашки")) {
                num_fishes = count;
            } else if (name == u8("Лодка")) {
                num_boats = count;
            } else if (name == u8("Кораблик")) {
                num_ships = count;
            } else if (name == u8("Пароходик")) {
                num_steamers = count;
            }
        }
    }
    canSend = false;
    c = body.findAll("FORM");
    foreach (QWebElement e, c) {
        QString do_cmd = e.findFirst("INPUT[name=do_cmd]").attribute("value");
        if (do_cmd == "send") { // форма отправления кораблика
            _formSend = e;
            canSend = true;
        } else if (do_cmd == "buy_auto"){ // форма отправления управляющего
            _formBuyAuto = e;
        } else if (do_cmd == "set_post"){ // форма выключения отчётов
            _formSetPost = e;
        } else if (do_cmd == "buy_ship"){ // форма покупки кораблика
            switch (e.findFirst("INPUT[name=ship]").attribute("value").toInt()) {
            case 2:
                _formBuyBoat = e;
                break;
            case 3:
                _formBuyBoat = e;
                break;
            case 4:
                _formBuySteamer = e;
                break;
            default:
                break;
            }
        }
    }
    if (!canSend) {
        parseTimerSpan(body.findFirst("SPAN#counter_1"),
                       &timeleft.pit, &timeleft.hms);
    }

    managed_till = QDateTime();
    with_manager = false;

    foreach (QWebElement e, body.findAll("DIV.inputGroup")) {
        if (e.findFirst("DIV.title").toPlainText()
            .trimmed() == u8("Управляющий")) {
            with_manager = (e.findFirst("INPUT[type=submit]")
                            .attribute("value") == u8("УВОЛИТЬ"));
            QRegExp rx("(\\d{2}):(\\d{2}) (\\d{2})\\.(\\d{2})\\.(\\d{4})");
            if (rx.indexIn(e.findFirst("FORM + P").toPlainText()) != -1) {
                qDebug("T={" + rx.cap(0) + "}");
                int hour    = rx.cap(1).toInt();
                int minute  = rx.cap(2).toInt();
                int day     = rx.cap(3).toInt();
                int month   = rx.cap(4).toInt();
                int year    = rx.cap(5).toInt();
                managed_till = QDateTime(QDate(year, month, day),
                                         QTime(hour, minute));
            }
        }
    }
}

QString Page_Game_Pier::toString (const QString& pfx) const {
    return "Page_Game_Pier {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString ("num_fishes   : %1\n").arg(num_fishes) +
            pfx + QString ("num_boats    : %1\n").arg(num_boats) +
            pfx + QString ("num_ships    : %1\n").arg(num_ships) +
            pfx + QString ("num_steamers : %1\n").arg(num_steamers) +
            pfx + (canSend ? "can send fishboat" : "can't send fisboat") + "\n" +
            pfx + QString ("timeleft     : %1\n").arg(timeleft.toString()) +
            pfx + (with_manager ? "with manager" : "witohut manager") + "\n" +
            pfx + QString("managed_till  : %1\n")
            .arg(managed_till.toString("yyyy-MM-dd hh:mm:ss")) +
            pfx + "}";
}

bool Page_Game_Pier::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Game_Pier");
    if (doc.findFirst("DIV[id=body] IMG.part_logo")
            .attribute("src").endsWith("/Harbour_2.jpg")) {
//        qDebug("Page_Game_Pier fits");
        return true;
    }
//    qDebug("Page_Game_Pier doesn't fits");
    return false;
}

bool Page_Game_Pier::doSend() {
    if (_formSend.isNull()) {
        qCritical("no _formSend");
        return false;
    }
    submit = _formSend.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("no submit input");
        return false;
    }
    qDebug("press send button");
    pressSubmit();
    return true;
}
