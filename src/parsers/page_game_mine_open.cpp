#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QStringList>
#include <QDebug>

#include "tools/tools.h"
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
    QWebElementCollection grbody = doc.findAll ("DIV[class=grbody]");

    // grbody[0] :: мини-купильня кирки, очков и каски
    // grbody[1] :: забой

    // распарсим купильню

    professional = !(grbody[0].findFirst("DIV[id=buy_block_1]").isNull());
    QWebElementCollection es = grbody [0].findAll ("TD");
    QWebElement e;
    int count = 0;

    //                "TABLE[class='default padding5'] TD");


    //    qDebug () << "TDS {";
    enum {
        Label,
        Pickaxe,
        Goggles,
        Helm,
        PickaxePro,
        GogglesPro,
        HelmPro
    } waiting4 = Label;

    static const QString tagPickaxe = u8 ("Кирка:");
    static const QString tagGoggles = u8 ("Очки:");
    static const QString tagHelm    = u8 ("Каска:");
    static const QString tagPickaxePro = u8 ("Кирка Трудяги:");
    static const QString tagGogglesPro = u8 ("Очки Трудяги:");
    static const QString tagHelmPro    = u8 ("Каска Трудяги:");

    num_pickaxes    = 0;
    num_goggles     = 0;
    num_helms       = 0;
    num_pickaxesPro = 0;
    num_gogglesPro  = 0;
    num_helmsPro    = 0;

    count = 0;
    foreach (e, es) {
        QString s = e.toPlainText ().trimmed ();

        //        qDebug () << count++ << ": " << s;

        if (waiting4 == Label) {
            if (s == tagPickaxe) {
                waiting4 = Pickaxe;
            } else if (s == tagGoggles) {
                waiting4 = Goggles;
            } else if (s == tagHelm) {
                waiting4 = Helm;
            } else if (s == tagPickaxePro) {
                waiting4 = PickaxePro;
            } else if (s == tagGogglesPro) {
                waiting4 = GogglesPro;
            } else if (s == tagHelmPro) {
                waiting4 = HelmPro;
            }
            continue;
        }
        int amount = rxVal (e);
        //qDebug () << "amount = " << amount;
        switch (waiting4) {
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
        case PickaxePro:
            num_pickaxesPro = amount;
            //            qDebug () << "num_pickaxes = " << amount;
            break;
        case GogglesPro:
            num_gogglesPro = amount;
            //            qDebug () << "num_goggles = " << amount;
            break;
        case HelmPro:
            num_helmsPro = amount;
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
    digstage = DigNone;
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
            break;
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
            pfx + "   professional:" + QVariant(professional).toString() + "\n" +
            pfx + "   pickaxes:" + QString::number (num_pickaxes) + "\n" +
            pfx + "   goggles :" + QString::number (num_goggles) + "\n" +
            pfx + "   helms   :" + QString::number (num_helms) + "\n" +
            pfx + "   pickaxes2:" + QString::number (num_pickaxesPro) + "\n" +
            pfx + "   goggles2 :" + QString::number (num_gogglesPro) + "\n" +
            pfx + "   helms2   :" + QString::number (num_helmsPro) + "\n" +
            pfx + "   stage   :" + ::toString (digstage) + "\n" +
            pfx + "   chance  :" + QString::number (success_chance) + "\n" +
            pfx + "   timer   :" + timer.toString () + "\n" +
            pfx + "}";
}

//static
bool Page_Game_Mine_Open::fit(const QWebElement& doc) {
    qDebug() << "* CHECK Page_Game_Mine_Open";
    QWebElement logo = doc.findFirst ("IMG[class=part_logo]");
    if (logo.isNull()) {
        return false;
    }
    QString logo_src = logo.attribute ("src");
    if (!logo_src.endsWith("Mine_Open.jpg")) {
        return false;
    }
    return true;
}

bool Page_Game_Mine_Open::doStart() {
    submit = document.findFirst("CENTER").findFirst("A");
    if (submit.isNull()) {
        qDebug() << "worklink not found";
        return false;
    }
    qDebug() << "click on worklink";
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Open::doCancel() {
    submit = document.findFirst("CENTER").findFirst("CENTER").findFirst("A");
    if (submit.isNull()) {
        qDebug() << "cancellink not found";
    }
    qDebug() << "click on cancellink";
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Open::doDig() {
    QWebElementCollection links = document.findFirst("CENTER").findAll("A");
    if (links.count() != 3) {
        qDebug() << "bad links count";
        return false;
    }
    submit = links[0];
    qDebug() << "click on diglink";
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Open::doReset() {
    QWebElementCollection links = document.findFirst("CENTER").findAll("A");
    if (links.count() != 3) {
        qDebug() << "bad links count";
        return false;
    }
    submit = links[1];
    qDebug() << "click on resetlink";
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Open::doQuit() {
    QWebElementCollection links = document.findFirst("CENTER").findAll("A");
    if (links.count() != 3) {
        qDebug() << "bad links count";
        return false;
    }
    submit = links[2];
    qDebug() << "click on quitlink";
    pressSubmit();
    return true;
}

bool Page_Game_Mine_Open::doQuickBuy(int position) {
    QWebElementCollection forms = document.findAll("FORM");
    if (position < 0 || forms.count() <= position) {
        qDebug() << "bad buying position";
        return false;
    }
    QWebElement form = forms[position];
    QWebElement currency1 = form.findFirst("INPUT[type=radio]");
    currency1.evaluateJavaScript("this.checked=true;");
    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qDebug() << "submit not found";
        return false;
    }
    qDebug() << u8("click on buylink, position %1").arg(position);
    pressSubmit();
    return true;
}


QString toString(const DigStage& stage) {
    switch (stage) {
    case DigNone: return "DigNone";
    case DigProcess: return "DigProcess";
    case DigReady: return "DigReady";
    }
    return "??? UNREACHABLE POINT ???";
}

