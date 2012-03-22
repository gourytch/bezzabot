#include <QEventLoop>
#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_incubator.h"
#include "tools/tools.h"

///
/// Page_Game_Incubator::Flying
///
//////////////////////////////////
Page_Game_Incubator::Flying::Flying() {
    rel         = -1;
    title       = "UNASSIGNED TITLE";
    kind        = "UNASSIGNED KIND";
    active      = false;
    was_born    = false;
    readiness   = -1;
    birth_pit   = PageTimer();
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
    was_born= !kind.endsWith('1');
    return true;
}

QString Page_Game_Incubator::Flying::toString() const {
    return u8("{rel=%1 title=%2 kind=%3 active=%4 born=%5}")
            .arg(rel).arg(title).arg(kind).arg(active)
            .arg(was_born ? "true" : "false");
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
            pfx + u8("fa_bonus  : %1\n").arg(fa_bonus.toString()) +
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
    rel_active = -1;
    ix_active = -1;
    int ix = -1;
    foreach (QWebElement e, document.findAll("DIV#flyings DIV[rel]")) {
        Flying item;
        ++ix;
        if (item.parse(e)) {
            flyings.append(item);
            if (item.active) {
                rel_active = item.rel;
                ix_active = ix;
            }
        }
    }
}


void Page_Game_Incubator::parseDivFlyingActions() {
    selectedTab = document.findFirst("DIV.flying_actions DIV.selected").attribute("id");
}


void Page_Game_Incubator::parseDivFlyingBlock() {
    QWebElement flying_block = document.findFirst("DIV#flying_block");
    fa_events0.reset();
    fa_boxgame.reset();
    fa_bonus.reset();

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
    if (fa_bonus.parse(flying_block)) {
        qDebug("fa_bonus detected");
        return;
    }
}


//////////////////////////////////////////////////////////////////////////////

///
/// Tab_Action_Normal
///
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


///
/// Tab_Action_Boxgame
///
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

///
/// Tab_Bonus
///

const char *Page_Game_Incubator::Tab_Bonus::bonus_name[8] = {
    "power", "block", "dexterity", "charisma",
    "safe", "safe2", "fast", "luck"
};


const char *Page_Game_Incubator::Tab_Bonus::bonus_name_r[8] = {
    "сила", "защита", "ловкость", "мастерство",
    "золотой_мешочек", "красный_мешочек", "колокольчик", "подкова"
};


const int Page_Game_Incubator::Tab_Bonus::bonus_price1[8] = {
    75, 75, 75, 75,
    75, 50, 250, 250
};

const int Page_Game_Incubator::Tab_Bonus::bonus_price2[8] = {
    15, 15, 15, 15,
    15, 10, 50, 50
};

void Page_Game_Incubator::Tab_Bonus::reset() {
    valid = false;
    block = QWebElement();
    checkboxes = QWebElementCollection();
    cooldowns.clear();
    submit = QWebElement();
}


bool Page_Game_Incubator::Tab_Bonus::parse(QWebElement flying_block) {
    reset();
    block = flying_block.findFirst("FORM.m0p0");
    if (block.isNull()) {
        return false;
    }
    checkboxes = block.findAll("DIV.bonus INPUT[type=checkbox]");
    if (checkboxes.count() != 8) {
        return false;
    }

    QWebElementCollection divs = block.findAll("DIV.bonus");
    if (divs.count() != 8) {
        return false;
    }

    for (int i = 0; i < divs.count(); ++i) {
        cooldowns.add(PageTimer(divs[i].findFirst("SPAN.js_timer")));
    }

    submit = block.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qDebug("? missing submit");
        return false;
    }
    valid = true;
    return true;
}

QString Page_Game_Incubator::Tab_Bonus::toString() const {
    if (!valid) return "?invalid bonus tab?";
    QString s;
    for (int i = 0; i < 8; ++i) {
        bool checked = checkboxes[i].attribute("checked") != "";
        QString cd;
        if (cooldowns[i].defined()) {
            int sec = QDateTime::currentDateTime().secsTo(cooldowns[i].pit);
            QString pit = ::toString(cooldowns[i].pit);
            if (sec > 0) {
                cd += u8("%1 seconds till %2").arg(sec).arg(pit);
            } else {
                cd += u8("expired since %1").arg(pit);
            }
        } else {
            cd = "unset";
        }
        s += u8("{#%1 %2, %3} ")
                .arg(i)
                .arg(u8(checked ? "checked" : "unchecked"))
                .arg(cd);
    }
    return cooldowns.toString();
}

////////////////////////////////////////////////////////////////////////////
// doXXXXX
////////////////////////////////////////////////////////////////////////////

bool Page_Game_Incubator::doStartSmallJourney(int duration10) {
    if (!fa_events0.valid) {
        qCritical("doStartSmallJourney() on invalid tab");
        return false;
    }
    submit = QWebElement();

    foreach (QWebElement form, document.findAll("DIV#flying_block FORM")) {
        if (form.findFirst("INPUT[name=do_cmd]").attribute("value") == "do_small") {

            submit = form.findFirst("INPUT[type=submit]");

            if (submit.isNull()) {
                qCritical("start-button not found");
                return false;
            }

            QWebElement selectedOpt;
            QString s10 = QString::number(duration10 * 10);
            foreach (QWebElement opt, form.findAll("SELECT[name=watch_time] OPTION")) {
                if (duration10 <= 0) {
                    selectedOpt = opt;
                    continue;
                }
                if (opt.attribute("value").trimmed() == s10) {
                    selectedOpt = opt;
                    break;
                }
            }
            if (selectedOpt.isNull()) {
                qCritical("select not found");
                return false;
            }
            selectedOpt.evaluateJavaScript("this.selected = true;");
            qWarning("Запускаю летуна в малое путешествие на %s минут",
                     qPrintable(selectedOpt.attribute("value").trimmed()));
            pressSubmit();
            return true;
        }
    }
    qCritical("small journey form not found");
    return false;
}


bool Page_Game_Incubator::doStartBigJourney() {
    if (!fa_events0.valid) {
        qCritical("doStartBigJourney() on invalid tab");
        return false;
    }
    submit = QWebElement();

    foreach (QWebElement form, document.findAll("DIV#flying_block FORM")) {
        if (form.findFirst("INPUT[name=do_cmd]").attribute("value") == "do_big") {
            submit = form.findFirst("INPUT[type=submit]");
            break;
        }
    }

    if (submit.isNull()) {
        qCritical("start-button not found");
        return false;
    }

    qWarning("Запускаю летуна в большое путешествие");
    pressSubmit();
    return true;
}


bool Page_Game_Incubator::doSelectBox(int boxNo) {
    QWebElementCollection chests = document.findAll("DIV#flying_block A.chest");
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
    QWebElement link= document.findFirst("DIV#flying_block CENTER A");
    if (link.isNull()) {
        qCritical("found no link");
        return false;
    }
    qDebug("close boxgame");
    submit = link;
    pressSubmit();
    qDebug("wait for reload");
    delay(3000 + (qrand() % 3000), false);
    qDebug("... reparse");
    reparse();
    return true;
}

bool Page_Game_Incubator::doSelectTab(const QString& tab, int timeout) {
    {
        QWebElement e = document.findFirst("DIV#" + tab);
        if (e.isNull()) {
            qCritical("MISSING TAB {%s}", qPrintable(tab));
            return false;
        }
        if (e.attribute("class").contains("selected")) {
            qDebug("tab %s already selected", qPrintable(tab));
            return true;
        }
        e = QWebElement();
    }
    qDebug("actuate tab " + tab);
    actuate(tab);
    delay((timeout < 0) ? 3000 + (qrand() % 3000) : timeout, false);
    QWebElement e = document.findFirst("DIV#" + tab);
    if (e.isNull()) {
        qCritical("TAB {%s} LOST", qPrintable(tab));
        return false;
    }
    if (!e.attribute("class").contains("selected")) {
        qCritical("tab %s was NOT selected", qPrintable(tab));
        qDebug("e = {%s}", qPrintable(e.toOuterXml()));
        return false;
    }
    qDebug("TAB SELECTED: %s, reparse and return", qPrintable(tab));
    reparse();
    return true;
}


int  Page_Game_Incubator::getBonusCooldown(int bonus_ix) {
    Q_ASSERT(bonus_ix >= 0 && bonus_ix < 8);
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return -1;
    }

    return fa_bonus.cooldowns[bonus_ix].cooldown();
}


bool Page_Game_Incubator::doBonusSetCheck(int bonus_ix, bool checked) {
    Q_ASSERT(bonus_ix >= 0 && bonus_ix < 8);
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }

    qDebug("we need checkbox #%d be %schecked",
           bonus_ix, checked ? "" : "un");

    if (checked != getBonusChecked(bonus_ix)) {
        qDebug("click to bonus checkbox #%d", bonus_ix);
        actuate(fa_bonus.checkboxes[bonus_ix]);
    }
    return true;
}


bool Page_Game_Incubator::getBonusChecked(int bonus_ix) {
    Q_ASSERT(bonus_ix >= 0 && bonus_ix < 8);
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    bool checked = fa_bonus.checkboxes[bonus_ix]
            .evaluateJavaScript("this.checked;").toBool();
    qDebug("bonus #%d is %schecked", bonus_ix, checked ? "" : "un");
    return checked;
}


int  Page_Game_Incubator::getBonusPrice1(int bonus_ix) {
    Q_ASSERT(bonus_ix >= 0 && bonus_ix < 8);
    return Tab_Bonus::bonus_price1[bonus_ix];
}


int  Page_Game_Incubator::getBonusPrice2(int bonus_ix) {
    Q_ASSERT(bonus_ix >= 0 && bonus_ix < 8);
    return Tab_Bonus::bonus_price1[bonus_ix];
}


int Page_Game_Incubator::getBonusTotalPrice1() {
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    qDebug("retrieve total price for currency #1");
    int n = dottedInt(document.findFirst("SPAN#bonus_money1")
                     .toPlainText().trimmed());
    qDebug("... = %d", n);
    return n;
}

int  Page_Game_Incubator::getBonusTotalPrice2() {
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    qDebug("retrieve total price for currency #2");
    int n = dottedInt(document.findFirst("SPAN#bonus_money2")
                     .toPlainText().trimmed());
    qDebug("... = %d", n);
    return n;
}


bool Page_Game_Incubator::doBonusSetCurrency(int ix) {
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    Q_ASSERT(ix >= 0 && ix < 2);
    qDebug("set currency #%d", ix);
    QWebElementCollection ptypes = document.findFirst("DIV#bonus_zoo_did")
            .findAll("INPUT[name=ptype]");
    actuate(ptypes[ix]);
    return true;
}


bool Page_Game_Incubator::doBonusSetDuration(int days) {
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    Q_ASSERT(days == 1 || days == 2 || days == 3 ||
             days == 7 || days == 14 || days == 28);
    qDebug("search for %d-days option", days);
    QString s = QString::number(days);
    foreach (QWebElement op, document.findAll("SELECT#buy_per_days OPTION")) {
        if (op.attribute("value").trimmed() == s) {
            qDebug("js-selecting %s", qPrintable(op.toOuterXml()));
            op.evaluateJavaScript("this.selected=true;");
            return true;
        }
    }
    qDebug("selected nothing");
    return false;
}


bool Page_Game_Incubator::doBonusSubmit() {
    if (!fa_bonus.valid) {
        qCritical("bonus tab is not active");
        return false;
    }
    QWebElement submit = document.findFirst("DIV#bonus_zoo_did INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    actuate(submit);
    return true;
}
