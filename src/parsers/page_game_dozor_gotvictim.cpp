#include <QWebElement>
#include <QWebElementCollection>
#include <QRegExp>
#include <QDebug>
#include "tools/tools.h"
#include "page_game_dozor_gotvictim.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Game_Dozor_GotVictim
//
////////////////////////////////////////////////////////////////////////////

Page_Game_Dozor_GotVictim::Page_Game_Dozor_GotVictim (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor_GotVictim;
    QWebElement tab = document.findFirst("TABLE.attack");
    QString img = tab.findFirst("TD[style]").attribute("style");
    is_scary = (img.indexOf("/monster/") > -1);
    if (is_scary) {
        QRegExp rx("/(.*)\\.jpg");
        if (rx.indexIn(img) != -1) {
            img_name = rx.cap(1);
        }
    }
    level       = -1;
    power       = -1;
    block       = -1;
    dexterity   = -1;
    endurance   = -1;
    charisma    = -1;
    foreach (QWebElement tr, tab.findAll("TABLE.skills TR")) {
        QWebElementCollection td = tr.findAll("TD");
        if (td.count() < 4) continue;
        QString name = td[1].toPlainText().trimmed();
        if (name == u8("Уровень")) {
            level = td[2].toPlainText().trimmed().toInt();
        } else if (name == u8("Сила")) {
            power = td[3].toPlainText().trimmed().toInt();
        } else if (name == u8("Защита")) {
            block = td[3].toPlainText().trimmed().toInt();
        } else if (name == u8("Ловкость")) {
            dexterity = td[3].toPlainText().trimmed().toInt();
        } else if (name == u8("Масса")) {
            endurance = td[3].toPlainText().trimmed().toInt();
        } else if (name == u8("Мастерство")) {
            charisma = td[3].toPlainText().trimmed().toInt();
        }
    }

    foreach (QWebElement e, body.findAll("INPUT[type=submit]")) {
        if (e.attribute("value") == u8("НАПАСТЬ")) {
            _attack = e;
        } else {
            _flee = e; //  hindicode!
        }
    }

}


QString Page_Game_Dozor_GotVictim::toString (const QString& pfx) const
{
    return "Page_Game_Dozor_GotVictim {\n" +

            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("VICTIM: %1 %2\n")
            .arg(is_scary ? "MONSTER" : "PLAYER")
            .arg(getName()) +
            pfx + QString("   level     : %1\n").arg(level) +
            pfx + QString("   power     : %1\n").arg(power) +
            pfx + QString("   block     : %1\n").arg(block) +
            pfx + QString("   dexterity : %1\n").arg(dexterity) +
            pfx + QString("   endurance : %1\n").arg(endurance) +
            pfx + QString("   charisma  : %1\n").arg(charisma) +
            pfx + "}";
}

QString Page_Game_Dozor_GotVictim::getName() const {
    if (is_scary) {
        return img_name;
    }
    return name;
}
//static
bool Page_Game_Dozor_GotVictim::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Game_Dozor_GotVictim");
    if (doc.findFirst("TABLE.attack").isNull()) {
        return false;
    }
    if (doc.findFirst("INPUT[type=submit]").attribute("value") != u8("НАПАСТЬ")) {
        return false;
    }
    return true;
}

bool Page_Game_Dozor_GotVictim::doAttack() {
    if (_attack.isNull()) {
        qCritical("submit for Attack is null");
        return false;
    }
    submit = _attack;
    pressSubmit();
    return true;
}

bool Page_Game_Dozor_GotVictim::doFlee() {
    if (_flee.isNull()) {
        qCritical("submit for Flee is null");
        return false;
    }
    submit = _flee;
    pressSubmit();
    return true;
}
