#include <QRegExp>
#include "page_game_house_plantation.h"
#include "tools/tools.h"


Page_Game_House_Plantation::Page_Game_House_Plantation(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_House_Plantation;
    QWebElement t = doc.findFirst("FORM TABLE");
    QString text_level = t.findFirst("TH").toPlainText().trimmed();
    QString text_info =  t.findFirst("TD").toPlainText().trimmed();

//    qDebug(u8("LEVEL {%1}").arg(text_level));
//    qDebug(u8("INFO  {%1}").arg(text_info));
    {
        QRegExp rx(u8("Текущий уровень: (\\d+)"));
        if (rx.indexIn(text_level) != -1) {
            plant_level = rx.cap(1).toInt();
        } else {
            plant_level = -1;
        }
    }

    {
        QRegExp rx(u8("Доход ([0123456789.+-]+)"));
        if (rx.indexIn(text_info) != -1) {
            plant_income = dottedInt(rx.cap(1));
        } else {
            plant_income = -1;
        }
    }

    {
        QRegExp rx(u8("мест для рабов – (\\d+)"));
        if (rx.indexIn(text_info) != -1) {
            plant_capacity = rx.cap(1).toInt();
        } else {
            plant_capacity = -1;
        }
    }

    {
        QRegExp rx(u8(" у вас: (\\d+)"));
        if (rx.indexIn(text_info) != -1) {
            plant_slaves = rx.cap(1).toInt();
        } else {
            plant_slaves = -1;
        }
    }
}


QString Page_Game_House_Plantation::toString (const QString& pfx) const {
    return "Page_Game_House_Plantation {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("plant level   : %1\n").arg(plant_level) +
            pfx + QString("plant income  : %1\n").arg(plant_income) +
            pfx + QString("plant capacity: %1\n").arg(plant_capacity) +
            pfx + QString("plant slaves  : %1\n").arg(plant_slaves) +
            pfx + "}";
}


bool Page_Game_House_Plantation::fit(const QWebElement& doc) {
    QString crumbs = doc.findFirst("DIV#crumbs").toPlainText().trimmed();
    if (crumbs == u8("Деревня > Жилище > Плантация")) {
        return true;
    }
//    qDebug(u8("CRUMBS:{%1}").arg(crumbs));
    return false;
}
