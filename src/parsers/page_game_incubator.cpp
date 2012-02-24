#include "page_game_incubator.h"
#include "tools/tools.h"

///
/// Page_Game_Incubator::Flying
///
//////////////////////////////////
Page_Game_Incubator::Flying::Flying() {
}


Page_Game_Incubator::Flying::Flying(const Flying& that) {
    *this = that;
}


const Page_Game_Incubator::Flying& Page_Game_Incubator::Flying::operator=(
        const Flying& that) {
    rel         = that.rel;
    title       = that.title;
    kind        = that.kind;
    active      = that.active;
    was_born    = that.was_born;
    readiness   = that.readiness;
    birth_pit   = that.birth_pit;

    return *this;
}

bool Page_Game_Incubator::Flying::parse(QWebElement& e) {
    if (e.tagName() != "DIV") return false;
    rel     = e.attribute("rel").toInt();
    title   = e.attribute("title");
    QRegExp rx("(flying\\d+)\\s+(active|passive)");
    if (rx.indexIn(e.attribute("class")) == -1) return false;
    kind    = rx.cap(1);
    active  = (rx.cap(2) == "active");
    return true;
}

QString Page_Game_Incubator::Flying::toString() const {
    return u8("{rel=%1 title=%2 kind=%3 active=%4}")
            .arg(rel).arg(title).arg(kind).arg(active);
}


Page_Game_Incubator::Page_Game_Incubator(QWebElement& doc) : Page_Game(doc) {
    pagekind = page_Game_Incubator;
    reparse();
}


QString Page_Game_Incubator::toString (const QString& pfx) const {
    QString s;
    for (int i = 0; i < flyings.count(); ++i) {
        const Flying &f = flyings[i];
        s += pfx + u8("   %1\n").arg(f.toString());

    }
    return "Page_Game_Incubator {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + u8("содержимое инкубатора:\n") +
            s +
            pfx + u8("вкладка: %1\n").arg(selectedTab) +
            pfx + u8("fa_events0: %1\n").arg(fa_events0.toString()) +
            pfx + u8("fa_boxgame: %1\n").arg(fa_boxgame.toString()) +
            pfx + "}";
}


bool Page_Game_Incubator::fit(const QWebElement& doc) {
    if (doc.findFirst("DIV#zoo_all").isNull()) {
        return false;
    }
    return true;
}

/////////////////////

void Page_Game_Incubator::reparse() {
    parseDivFlyings();
    parseDivFlyingActions();
    parseDivFlyingBlock();
}


void Page_Game_Incubator::parseDivFlyings() {
    flyings.clear();
    foreach (QWebElement e, document.findAll("DIV#flyings DIV[rel]")) {
        Flying item;
        if (item.parse(e))  flyings.append(item);
    }

}


void Page_Game_Incubator::parseDivFlyingActions() {
    selectedTab = document.findFirst("DIV.flying_actions DIV.selected").attribute("id");
}


void Page_Game_Incubator::parseDivFlyingBlock() {
    QWebElement flying_block = document.findFirst("DIV#flying_block");
    fa_events0.reset();
    fa_boxgame.reset();
    if (flying_block.isNull()) {
        qCritical("flying_block not found");
        return;
    }
    if (fa_events0.parse(flying_block)) {
        qDebug("fa_events0 detected");
        return;
    }
    if (fa_boxgame.parse(flying_block)) {
        qDebug("fa_boxgame detected");
        return;
    }
}


//////////////////////////////////////////////////////////////////////////////


void Page_Game_Incubator::Tab_Action_Normal::reset() {
    block = QWebElement();
    valid = false;
    minutesleft = -1;
}


bool Page_Game_Incubator::Tab_Action_Normal::parse(QWebElement flying_block) {
    reset();
    block = flying_block;
    QWebElement e = block.findFirst("DIV.zoo_event_1");
    if (e.isNull()) return false;
    if (e.attribute("title") != u8("Маленькое приключение")) return false;
    minutesleft = 0;
    foreach (e, block.findAll("SELECT#watch_time OPTION")) {
        int v = e.attribute("value").toInt();
        if (minutesleft < v) minutesleft = v;
    }
    valid = true;
    return true;
}


QString Page_Game_Incubator::Tab_Action_Normal::toString() const {
    if (!valid) return "?invalid Tab_Action_Normal?";
    return u8("Events{minutesleft:%1").arg(minutesleft);
}


void Page_Game_Incubator::Tab_Action_Boxgame::reset() {
    block       = QWebElement();
    valid       = false;
    is_finished = false;
    box_no      = -1;
    currency    = QString();
    amount      = -1;
}

bool Page_Game_Incubator::Tab_Action_Boxgame::parse(QWebElement flying_block) {
    block = flying_block;
    QWebElementCollection chests = block.findAll("A.chest");
    num_chests = chests.count();
    if (num_chests > 0) {
        is_finished = false;
        valid = true;
        return true;
    }
    chests = block.findAll("I.chest");
    num_chests = chests.count();
    if (num_chests == 0) return false;
    is_finished = true;
    QRegExp rx1(u8("opened_\\d+_1"));
    QRegExp rx2(u8("(золото|пирашки|кристаллы): (\\d+)"));
    foreach (QWebElement e, chests) {
        if ((rx1.indexIn(e.attribute("class")) != -1) &&
            (rx2.indexIn(e.attribute("title")) != -1)) {
            currency    = rx2.cap(1);
            amount      = rx2.cap(2).toInt();
            break;
        }
    }
    valid = true;
    return true;
}

QString Page_Game_Incubator::Tab_Action_Boxgame::toString() const {
    if (!valid) return "?invalid boxgame?";
    if (is_finished) {
        return u8("boxgame finished %1: %2").arg(currency).arg(amount);
    }
    return u8("boxgame started. %1 boxes").arg(num_chests);
}

////////////////////////////////////////////////////////////////////////////
// doXXXXX
////////////////////////////////////////////////////////////////////////////

bool Page_Game_Incubator::doStartBigJourney() {
    if (!fa_events0.valid) {
        qCritical("doStartBigJourney() on invalid tab");
        return false;
    }
    submit = fa_events0.block.findFirst("DIV.zoo_event_2 FORM INPUT.cmd_all");
    if (submit.isNull()) {
        qCritical("start-button not found");
        return false;
    }

    qWarning("Запускаю летуна в большое путешествие");
    pressSubmit();
    return true;
}


bool Page_Game_Incubator::doSelectBox(int boxNo) {
    QWebElementCollection chests = document.findAll("DIV#flying_stop A.chest");
    if (chests.count() == 0) {
        qCritical("found no chests");
        return false;
    }
    if (boxNo >= chests.count()) {
        qCritical("boxNo is out ouf bounds");
        return false;
    }
    if (boxNo < 0) boxNo = qrand() % chests.count();
    submit = chests[boxNo];
    qDebug("open box #%d", boxNo);
    pressSubmit();
    return true;
}


bool Page_Game_Incubator::doFinishGame() {
    QWebElement link= document.findFirst("DIV#flying_stop A");
    if (link.isNull()) {
        qCritical("found no link");
        return false;
    }
    submit = link;
    qDebug("to next journeys");
    pressSubmit();
    return true;
}
