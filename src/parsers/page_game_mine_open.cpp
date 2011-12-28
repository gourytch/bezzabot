#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "tools.h"
#include "types.h"
#include "page_game_mine_open.h"


int rxVal (const QWebElement& e)
{
    QString s = e.toPlainText ().trimmed ();
    QRegExp rx ("(\\d+)");
    return (rx.indexIn (s) == -1) ? -1 : rx.cap (1).toInt ();
}


Page_Game_Mine_Open::Page_Game_Mine_Open (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Mine_Open;
    QWebElementCollection grbody = doc.findAll ("DIV.grbody");

    // grbody[0] :: мини-купильня кирки, очков и каски
    // grbody[1] :: забой

    // распарсим купильню
    QWebElementCollection es = grbody [0].findAll ("TD");
    QWebElement e;
    int count = 0;

//                "TABLE[class='default padding5'] TD");


//    qDebug () << "TDS {";
    enum {Label, Pickaxe, Goggles, Helm} waiting4 = Label;

    static const QString tagPickaxe = u8 ("Кирка:");
    static const QString tagGoggles = u8 ("Очки:");
    static const QString tagHelm    = u8 ("Каска:");

    count = 0;
    foreach (e, es)
    {
        QString s = e.toPlainText ().trimmed ();

//        qDebug () << count++ << ": " << s;

        if (waiting4 == Label)
        {
            if (s == tagPickaxe)
            {
                waiting4 = Pickaxe;
            }
            else if (s == tagGoggles)
            {
                waiting4 = Goggles;
            }
            else if (s == tagHelm)
            {
                waiting4 = Helm;
            }
            continue;
        }
        int amount = rxVal (e);
        //qDebug () << "amount = " << amount;
        switch (waiting4)
        {
        case Pickaxe:
            num_pickaxes = amount;
//            qDebug () << "num_pickaxes = " << amount;
            break;
        case Goggles:
            num_goggles = amount;
//            qDebug () << "num_goggles = " << amount;
            break;
        case Helm:
            num_helms = amount;
//            qDebug () << "num_helms = " << amount;
            break;
        case Label:
            qDebug () << "??? WTF";
        }
        waiting4 = Label;
    }
//    qDebug () << "}";

    QRegExp rx (u8 ("Вероятность успеха.*(\\d+)"));
    es = doc.findAll ("P");
    count = 0;
    digstage = DigReady;
    success_chance = -1;
    foreach (e, es)
    {
        QString s = e.toPlainText ().trimmed ();
//        qDebug () << "[" << count++ << "] = " << s;
        if (s == u8 ("Вы ищете кристаллы"))
        {
            parseTimerSpan (doc.findFirst ("SPAN[id=counter_1]"),
                            &(timer.pit), &(timer.hms));
            digstage = DigProcess;
        }
        if (rx.indexIn (s) > -1)
        {
            success_chance = rx.cap (1).toInt ();
            digstage = DigReady;
            break;
        }
    }

    es = doc.findAll ("DIV[class=grbody]");
    count = 0;
    foreach (e, es)
    {
        QString s = e.toPlainText ().trimmed ();
        qDebug () << "[" << count++ << "] = " << s;
    }

}


//virtual
QString Page_Game_Mine_Open::toString (const QString &pfx) const
{
    return "Page_Game_Mine_Open {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "   pickaxes:" + QString::number (num_pickaxes) + "\n" +
            pfx + "   goggles :" + QString::number (num_goggles) + "\n" +
            pfx + "   helms   :" + QString::number (num_helms) + "\n" +
            pfx + "   stage   :" + QString::number (digstage) + "\n" +
            pfx + "   chance  :" + QString::number (success_chance) + "\n" +
            pfx + "   timer   :" + timer.toString () + "\n" +
            pfx + "}";
}

