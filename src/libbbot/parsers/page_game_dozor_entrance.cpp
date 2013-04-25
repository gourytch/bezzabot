#include <QWebElement>
#include <QString>
#include <QWebElementCollection>
#include <QDebug>
#include "page_game_dozor_entrance.h"
#include "tools/tools.h"

////////////////////////////////////////////////////////////////////////////
//
// Page_Game_Dozor_Entrance
//
////////////////////////////////////////////////////////////////////////////

Page_Game_Dozor_Entrance::Page_Game_Dozor_Entrance (QWebElement& doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Dozor_Entrance;
    QWebElementCollection groups = document.findAll("DIV#body DIV.grbody");
//    QWebElementCollection groups = document.findAll("DIV#body TABLE.default TD.half DIV.inputGroup");
    // groups::
    // [0] - писанина про пятую точку
    // [1] - бодалка
    // [2] - дозор
    // [3] - Zorro
    // [4] - Страшилки

    if (groups.count() != 5) {
        qCritical(u8("Page_Game_Dozor_Entrance: groups.size=%1").arg(groups.count()));
        return;
    }
    _fightForm = groups[1];
    _dozorForm = groups[2];
    _zorroForm = groups[3];
    _scaryForm = groups[4];

    // обработаем поиск противника
    QWebElement e = _fightForm.findFirst("DIV.watch_no_attack");
    if (e.isNull()) { // нет запрета на бодалку
        fight_price = dottedInt(
                    _fightForm.findFirst("P.sub_title_text B")
                    .toPlainText(), NULL);
    } else { // должен быть откат бодалки
        fight_price = -1;
        e = _fightForm.findFirst("SPAN[timer]");
        if (e.isNull()) {
            qCritical(u8("Page_Game_Dozor_Entrance: SPAN[timer] for fightForm was not found"));
        } else {
            fight_cooldown.assign(e);
        }
    }

    // обработаем дозор
    dozor_price = dottedInt(
                _dozorForm.findFirst("SPAN.price_num")
                .toPlainText(), NULL);
    QWebElement selector = _dozorForm.findFirst("SELECT#auto_watch");
    QWebElementCollection options = selector.findAll("OPTION");
    dozor_left10 = 0;
    foreach (QWebElement e, options) {
        int v = e.attribute("value").toInt();
        if (dozor_left10 < v) {
            dozor_left10 = v;
        }
    }

    // обработаем zorro
    // TODO: найти и обработать форму без активной маски
    e = _zorroForm.findFirst("DIV.watch_no_attack");
    if (e.isNull()) { // нет запрета на Zorro
        zorro_price = dottedInt(
                    _zorroForm.findFirst("P.sub_title_text B")
                    .toPlainText(), NULL);
    } else { // должен быть откат
        zorro_price = -1;
        e = _zorroForm.findFirst("SPAN[timer]");
        if (e.isNull()) {
            qCritical(u8("Page_Game_Dozor_Entrance: SPAN[timer] for zorroForm was not found"));
        } else {
            zorro_cooldown.assign(e);
        }
    }

    // обработаем страшилок
    e = _scaryForm.findFirst("SPAN[timer]");
    if (e.isNull()) {
        scary_auto_price = dottedInt(
                    _scaryForm.findFirst("P.sub_title_text B")
                    .toPlainText(), NULL);
    } else {
        scary_auto_price = -1;
        scary_cooldown.assign(e);
    }


}


QString Page_Game_Dozor_Entrance::toString (const QString& pfx) const
{
    return "Page_Game_Dozor_Entrance {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "scary cooldown: " +
            (scary_cooldown.active()
             ? scary_cooldown.toString()
             : QString("inactive")) + "\n" +
            pfx + "БОДАЛКА:\n" +
            pfx + "   стоимость: " + QString::number(fight_price) + "\n" +
            pfx + "       откат: " +
            (fight_cooldown.active()
             ? fight_cooldown.toString()
             : QString("inactive")) + "\n" +
            pfx + "ДОЗОР:\n" +
            pfx + "   стоимость: " + QString::number(dozor_price) + " з.(?)\n" +
            pfx + "    осталось: " + QString::number(dozor_left10) + " * 10 мин\n" +
            pfx + "ZORRO: " + u8(zorro_enabled ? "активен" : "неактивен") + "\n" +
            pfx + "   стоимость: " + QString::number(zorro_price) + "\n" +
            pfx + "       откат: " +
            (zorro_cooldown.active()
             ? zorro_cooldown.toString()
             : QString("inactive")) + "\n" +
            pfx + "СТРАШИЛКИ:\n" +
            pfx + "   стоимость: " + QString::number(scary_auto_price) + "\n" +
            pfx + "       откат: " +
            (scary_cooldown.active()
             ? scary_cooldown.toString()
             : QString("inactive")) + "\n" +
            pfx + "}";
}

//static
bool Page_Game_Dozor_Entrance::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Game_Dozor_Entrance");
    QWebElementCollection titles = doc.findAll ("DIV[class=title]");
    if (!titles.count ()) {
//        qDebug("Page_Game_Dozor_Entrance doesn't fit: no titles");
        return false;
    }
    foreach (QWebElement e, titles) {
        QString title = e.toPlainText ().trimmed ();
        if (title == u8("Капитан стражи")) {
//            qDebug("Page_Game_Dozor fit: Капитан Стражи detected");
            return true;
        }
    }
//    qDebug("Page_Game_Dozor_Entrance doesn't fit");
    return false;
}

bool Page_Game_Dozor_Entrance::doDozor(int time10) {
    if (dozor_left10 < time10) {
        qCritical("превышение времени дозора");
        return false;
    }
    if (gold < dozor_price) {
        qCritical("нехватка золота на дозор");
        return false;
    }
    QWebElement selector = _dozorForm.findFirst("SELECT[id=auto_watch]");
    QWebElementCollection options = selector.findAll("OPTION");
    bool valSet = false;
    foreach (QWebElement e, options) {
        if (e.attribute("value").toInt() == time10) {
            e.evaluateJavaScript("this.selected = true;");
            valSet = true;
            break;
        }
    }
    if (!valSet) {
        qCritical("опция не найдена");
        return false;
    }

    submit = _dozorForm.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("кнопка старта не найдена");
        return false;
    }
//    qWarning("нажимаем кнопку старта дозора");
    qDebug("нажимаем кнопку старта дозора");
    pressSubmit();
    return true;
}

bool Page_Game_Dozor_Entrance::doScarySearch(int ix) {
    if (_scaryForm.isNull()) {
        qCritical("scaryForm is null!");
        return false;
    }
    if (scary_cooldown.active()) {
        qCritical("scary_cooldown is active!");
        return false;
    }
    QWebElementCollection forms = _scaryForm.findAll("FORM");
    Q_ASSERT(forms.count() == 2);

    if (ix == 0) { // ищем автоматом за золото
        if (gold < scary_auto_price) {
            qCritical("not enough gold");
            return false;
        }
        submit = forms[0].findFirst("FORM INPUT[type=submit]");
        if (submit.isNull()) {
            qCritical("submit not found");
            return false;
        }
        if (submit.attribute("value") != u8("ИСКАТЬ СТРАШИЛКУ")) {
            qCritical("not my submit");
            return false;
        }

        qDebug("press on autosearch for scary");
        pressSubmit();
        return true;
    }
    QWebElement form = forms[1];

    //select cryst
    form.findAll("INPUT[name=ptype]")[0].evaluateJavaScript("this.selected=true;");

    QWebElementCollection levels = form.findAll("SELECT OPTION");
    Q_ASSERT(levels.count() > 2);
    if (ix == -1) { // ищем бандюка
        levels[levels.count()-1].evaluateJavaScript("this.selected=true;");
    } else {
        if (ix > levels.count()-1) {
            qCritical("requested level is too high");
            return false;
            levels[ix].evaluateJavaScript("this.selected=true;");
        }
    }
    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    if (submit.attribute("value") != u8("АТАКА")) {
        qCritical("not my submit");
        return false;
    }
    qDebug("press on submit");
    pressSubmit();
    return true;
}


bool Page_Game_Dozor_Entrance::doFightSearch(QString /* attack_type */) {
    // REM: ix
    if (_fightForm.isNull()) {
        qCritical("fightForm is null!");
        return false;
    }
    if (fight_cooldown.active()) {
        qCritical("fight_cooldown is active!");
        return false;
    }
    QWebElementCollection forms = _fightForm.findAll("FORM");

    Q_ASSERT(forms.count() == 2);

    QWebElement form = forms[0]; // базовая атака

    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    if (submit.attribute("value") != u8("АТАКА")) {
        qCritical("not my submit");
        return false;
    }
    qDebug("press on submit");
    pressSubmit();
    return true;
}


bool Page_Game_Dozor_Entrance::doZorroSearch(QString /* attack_type */) {
    // REM: ix
    if (_zorroForm.isNull()) {
        qCritical("zorroForm is null!");
        return false;
    }
    if (zorro_cooldown.active()) {
        qCritical("zorro_cooldown is active!");
        return false;
    }
    QWebElementCollection forms = _zorroForm.findAll("FORM");

    QWebElement form = forms[0]; // базовый поиск

    submit = form.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    if (submit.attribute("value") != u8("ПОИСК")) {
        qCritical("not my submit. value=" + submit.attribute("value"));
        return false;
    }
    qDebug("press on submit");
    pressSubmit();
    return true;
}
