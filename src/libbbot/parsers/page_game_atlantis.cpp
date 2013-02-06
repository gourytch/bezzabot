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
//    qDebug("parse bathyscaphe from : " + info.toOuterXml());
    if (!info.findFirst("DIV.bathyscaphe-build-info").isNull()) {
        QWebElement prc = info.findFirst("SPAN.hangar-price");
        Q_ASSERT(!prc.isNull());
        QWebElement c = prc.firstChild();
        Q_ASSERT(c.tagName() == "SPAN");
        price = dottedInt(c.toInnerXml());
        c = c.nextSibling();
//        qDebug(u8("nextSibling tag={%1}").arg(c.tagName()));
        Q_ASSERT(c.tagName() == "B");
        currency = c.attribute("title");
        state = Unpaid;
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
        s = u8("стоимость      : %1, %2").arg(currency).arg(price);
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
    QWebElement e;

    e = document.findFirst("SPAN#total_bathyscaphes_count");
    Q_ASSERT(!e.isNull());
    total_bathyscaphes_count = dottedInt(e.toPlainText());

    e = document.findFirst("SPAN.big-timer SPAN.js_timer");
    Q_ASSERT(!e.isNull());
    atlantis_reachable_cooldown.assign(e);

    e = document.findFirst("SPAN#atlant_amount");
    Q_ASSERT(!e.isNull());
    atlant_amount = dottedInt(e.toPlainText());

    e = document.findFirst("DIV#atlantis-secret");
    Q_ASSERT(!e.isNull());
    boxgame = isDisplayed(e);

    e = document.findFirst("SPAN.hangar-count");
    Q_ASSERT(!e.isNull());
    hangar_count = dottedInt(e.toPlainText());

    e = document.findFirst("SPAN.hangar-max-count");
    Q_ASSERT(!e.isNull());
    hangar_max_count = dottedInt(e.toPlainText());

    e = document.findFirst("SPAN.working-count");
    Q_ASSERT(!e.isNull());
    working_count = dottedInt(e.toPlainText());

    foreach(QWebElement b, document.findAll("DIV.bathyscaphes DIV.bathyscaphe-info")) {
        if (!isDisplayed(b)) {
            continue;
        }
        Bathyscaphe bs;
        bs.parse(b);
        bathyscaphes.append(bs);
    }

    return true;
}


bool Page_Game_Atlantis::canBuyBathyscaphe() {
    foreach (Bathyscaphe bs, bathyscaphes) {
        if (bs.state != Unpaid) continue;

        if (bs.currency == u8("Золото")) {
            if (gold < bs.price) return false;
            return true;
        }
        if (bs.currency == u8("Кристаллы")) {
            if (crystal < bs.price) return false;
            return true;
        }
        return false;
    }
    return false;
}


bool Page_Game_Atlantis::doBuyBathyscaphe() {
    if (!canBuyBathyscaphe()) {
        qCritical("батискаф купить нельзя!");
        return false;
    }
    QWebElement e = document.findFirst("DIV.bathyscaphe-build-info INPUT[type=submit]");
    if (e.isNull()) {
        qCritical("кнопка покупки батискафа не найдена!");
        return false;
    }
    if (!isDisplayed(e)) {
        qCritical("кнопка покупки батискафа спрятана!");
        return false;
    }
    if (e.attribute("class").contains("cmd_blocked")) {
        qCritical("кнопка покупки батискафа заблокирована!");
        return false;
    }
    qWarning("покупаем новый батискаф");
    actuate(e);
    return true;
}


bool Page_Game_Atlantis::canLaunchBathyscaphe() {
    return false;
}

bool Page_Game_Atlantis::doLaunchBathyscaphe() {
    return false;
}
