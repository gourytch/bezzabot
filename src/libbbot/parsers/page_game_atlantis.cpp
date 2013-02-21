#include <QWebElement>
#include "page_game_atlantis.h"
#include "tools/tools.h"

ESTART(BathyState)
ECASE(Unpaid)
ECASE(Awaiting)
ECASE(Building)
ECASE(Diving)
EEND

bool Page_Game_Atlantis::Bathyscaphe::parse(QWebElement& info) {
    price = -1;
    currency = currency_Undefined;
//    qDebug("parse bathyscaphe from : " + info.toOuterXml());
    if (!info.findFirst("DIV.bathyscaphe-build-info").isNull()) {
        state = Unpaid;
        QWebElement prc = info.findFirst("SPAN.hangar-price");
        Q_ASSERT(!prc.isNull());
        QWebElement c = prc.firstChild();
        Q_ASSERT(c.tagName() == "SPAN");
        price = dottedInt(c.toInnerXml(), NULL);
        c = c.nextSibling();
//        qDebug(u8("nextSibling tag={%1}").arg(c.tagName()));
        Q_ASSERT(c.tagName() == "B");
        QString s = c.attribute("title");
        if (s == u8("Золото")) {
            currency = currency_Gold;
        } else if (s == u8("Кристаллы")) {
            currency = currency_Crystals;
        } else if (s == u8("Зелень")) {
            currency = currency_Green;
        } else {
            qFatal(u8("не могу распознать валюту: %1").arg(s));
            currency = currency_Undefined;
        }
    } else {
        QWebElement ct = info.findFirst("SPAN.bathyscaphe-status");
        Q_ASSERT(!ct.isNull());
        QWebElement t = ct.findFirst("DIV.status-timer");
        if (t.isNull()) {
            state = Awaiting;
        } else {
            QString s = t.toInnerXml();
            QWebElement jst = t.findFirst("SPAN.js_timer");
            Q_ASSERT(!jst.isNull());
            cooldown.assign(jst);
            if (s.contains(u8("Время спуска:"))) {
                state = Diving;
            } else {
                state = Building;
            }
        }
    }
    return true;
}

QString Page_Game_Atlantis::Bathyscaphe::toString(const QString &pfx) const {
    QString s;
    switch (state) {
    case Unpaid:
        s = u8("стоимость      : %1, %2").arg(::toString(currency)).arg(price);
        break;
    case Awaiting:
        s = "";
        break;
    case Building:
        s = u8("до изготовления: %1").arg(cooldown.toString());
        break;
    case Diving:
        s = u8("до всплытия    : %1").arg(cooldown.toString());
        break;
    default:
        s = u8("эээ?");
        break;
    }
    return pfx + u8("{state: %1, %2}").arg(::toString(state)).arg(s);
}


Page_Game_Atlantis::Page_Game_Atlantis(QWebElement& doc) : Page_Game(doc) {
    parse();
    pagekind = page_Game_Atlantis;
}


QString Page_Game_Atlantis::toString(const QString& pfx) const {
    QString s = "   состояние батискафов:";
    foreach (Bathyscaphe bs, bathyscaphes) {
        s += bs.toString("\n      ");
    }
    if (boxgame) {
        s += "\n   BOXGAME";
    }
    return "Page_Game_Atlantis {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("   всего батискафов в море      : %1\n")
            .arg(total_bathyscaphes_count) +
            pfx + u8("   таймер доступности атлантиды : %1\n")
            .arg(atlantis_reachable_cooldown.toString()) +
            pfx + u8("   атлантов на счету   : %1\n").arg(atlant_amount) +
            pfx + u8("   заполнение ангара   : %1 из %2\n")
            .arg(hangar_count).arg(hangar_max_count) +
            pfx + u8("   активных батискафов : %1\n").arg(working_count) +
            s + "\n" +
            pfx + "}";
}

bool Page_Game_Atlantis::fit(const QWebElement& doc) {
    if (doc.findFirst("SPAN#total_bathyscaphes_count").isNull()) {
        return false;
    }
    return true;
}

bool Page_Game_Atlantis::parse() {
    if (!parseHead()) return false;
    if (!parsePier()) return false;
    if (!parseSecret()) return false;
    if (!parseSea()) return false;

    return true;
}


bool Page_Game_Atlantis::parseHead() {
    QWebElement e;

    e = document.findFirst("SPAN#total_bathyscaphes_count");
    Q_ASSERT(!e.isNull());
    total_bathyscaphes_count = dottedInt(e.toPlainText(), NULL);

    e = document.findFirst("SPAN.big-timer SPAN.js_timer");
    Q_ASSERT(!e.isNull());
    atlantis_reachable_cooldown.assign(e);

    e = document.findFirst("SPAN#atlant_amount");
    Q_ASSERT(!e.isNull());
    atlant_amount = dottedInt(e.toPlainText(), NULL);

    e = document.findFirst("SPAN.hangar-count");
    Q_ASSERT(!e.isNull());
    hangar_count = dottedInt(e.toPlainText(), NULL);

    e = document.findFirst("SPAN.hangar-max-count");
    Q_ASSERT(!e.isNull());
    hangar_max_count = dottedInt(e.toPlainText(), NULL);

    e = document.findFirst("SPAN.working-count");
    Q_ASSERT(!e.isNull());
    working_count = dottedInt(e.toPlainText(), NULL);

    return true;
}


bool Page_Game_Atlantis::parsePier() {
    button_launch_bathyscaphe   = QWebElement();
    QWebElement pier = document.findFirst("DIV#pier");
    QWebElement e =  pier.findFirst("FORM INPUT[type=submit]");
    if (!e.isNull() && isDisplayed(e) && !isBlocked(e)) {
        button_launch_bathyscaphe = e;
        qDebug("button_launch_bathyscaphe: " + e.toOuterXml());
    }
    return true;
}


bool Page_Game_Atlantis::parseSecret() {
    link_chests = QWebElementCollection();

    QWebElement secret = document.findFirst("DIV#atlantis-secret");
    Q_ASSERT(!secret.isNull());
    boxgame = isDisplayed(secret);

    if (boxgame) {
        link_chests = secret.findAll("A");
        qDebug("chests:");
        foreach (QWebElement e, link_chests) {
            qDebug("chest: " + e.toOuterXml());
        }
        qDebug("---");
    }
    return true;
}


bool Page_Game_Atlantis::parseSea() {
    button_build_bathyscaphe = QWebElement();
    bathyscaphes.clear();

    QWebElement sea = document.findFirst("DIV#sea");

    foreach(QWebElement b, sea.findAll("DIV.bathyscaphe-info")) {
        if (!isDisplayed(b)) {
            continue;
        }
        Bathyscaphe bs;
        bs.parse(b);
        bathyscaphes.append(bs);
    }

    QWebElement e = sea.findFirst("FORM INPUT[type=submit]");
    if (!e.isNull() && isDisplayed(e) && !isBlocked(e)) {
        button_launch_bathyscaphe = e;
        qDebug("button_launch_bathyscaphe: " + e.toOuterXml());
    }
    return true;
}


bool Page_Game_Atlantis::canBuyBathyscaphe() {
    if (!parseSea()) return false;

    if (next_bathyscaphe_price == -1 ||
        next_bathyscaphe_currency == currency_Undefined) {
        return false;
    }

    if (button_build_bathyscaphe.isNull()) return false;

    switch (next_bathyscaphe_currency) {
    case currency_Gold:
        return (next_bathyscaphe_price <= gold);
    case currency_Crystals:
        return (next_bathyscaphe_price <= crystal);
    case currency_Green:
        return (next_bathyscaphe_price <= green);
    default:
        return false;
    }

    return false;
}


bool Page_Game_Atlantis::doBuyBathyscaphe() {
    if (!canBuyBathyscaphe()) {
        qCritical("батискаф купить нельзя!");
        return false;
    }
    qWarning("покупаем новый батискаф");
    actuate(button_build_bathyscaphe);
    return true;
}


bool Page_Game_Atlantis::canLaunchBathyscaphe() {
    if (!parsePier()) return false;
    if (button_launch_bathyscaphe.isNull()) return false;
    return true;
}


bool Page_Game_Atlantis::doLaunchBathyscaphe() {
    if (!canBuyBathyscaphe()) {
        qCritical("батискаф запустить нельзя!");
        return false;
    }
    qWarning("запускаем батискаф");
    actuate(button_build_bathyscaphe);
    return true;
}


QDateTime Page_Game_Atlantis::findMinDivingCooldown() {
    parseSea();
    QDateTime min;
    foreach (Bathyscaphe bs, bathyscaphes) {
        switch (bs.state) {
        case Awaiting:
            return QDateTime::currentDateTime();
        case Diving:
            if (min.isNull() || min < bs.cooldown.pit) {
                min = bs.cooldown.pit;
            }
            break;
        default:
            break;
        }
    }
    return min;
}
