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
    // DIV[id=body]
    QWebElement infotab = body.findFirst("TABLE[class=info]");
    QWebElementCollection groups = infotab.findAll("DIV[class=grbody]");

    _dozorForm = groups[1];
    dozor_price = dottedInt(
                _dozorForm.findFirst("SPAN[class=price_num]")
                .toPlainText());
    QWebElement selector = _dozorForm.findFirst("SELECT[id=auto_watch]");
    QWebElementCollection options = selector.findAll("OPTION");
    dozor_left10 = 0;
    foreach (QWebElement e, options) {
        int v = e.attribute("value").toInt();
        if (dozor_left10 < v) {
            dozor_left10 = v;
        }
    }

    _scaryForm = groups[2];
    QWebElement t = _scaryForm.findFirst("SPAN.js_timer");
    if (t.isNull()) {
        scary_auto_price = dottedInt(
                    _scaryForm.findFirst("SPAN[class=price_num]")
                    .toPlainText());
    } else {
        scary_auto_price = -1;
        scary_cooldown.assign(t);
    }


}


QString Page_Game_Dozor_Entrance::toString (const QString& pfx) const
{
    return "Page_Game_Dozor_Entrance {\n" +
            pfx + Page_Game::toString (pfx + "   ") + "\n" +
            pfx + "dozor_price: " + QString::number(dozor_price) + "\n" +
            pfx + "dozor_left10: " + QString::number(dozor_left10) + "\n" +
            pfx + "scary cooldown: " +
            (scary_cooldown.active()
             ? scary_cooldown.toString()
             : QString("inactive")) + "\n" +
            pfx + "scary_auto_price: " + QString::number(scary_auto_price) + "\n" +
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
    if (submit.attribute("value") != u8("ИСКАТЬ")) {
        qCritical("not my submit");
        return false;
    }
    qDebug("press on submit");
    pressSubmit();
    return true;
}
