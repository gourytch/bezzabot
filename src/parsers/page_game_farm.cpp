#include <QWebElement>
#include <QDebug>
#include "tools/tools.h"
#include "types.h"
#include "page_game_farm.h"


Page_Game_Farm::Page_Game_Farm (QWebElement &doc) :
    Page_Game (doc)
{
    pagekind = page_Game_Farm;
    QString s = body.findFirst("SPAN.price_num").toPlainText().trimmed();
    if (!s.isEmpty()) {
        working = false;
        price = s.toInt();
        maxhours = 0;
        foreach (QWebElement e, body.findAll("SELECT[id=work] OPTION")) {
            int n = e.attribute("value").trimmed().toInt();
            if (n > maxhours) {
                maxhours = n;
            }
        }
    } else {
        price = -1;
        maxhours = -1;
        working = true;
        parseTimerSpan(body.findFirst("SPAN.js_timer"),
                       &worktimer.pit, &worktimer.hms);
        worktimer.adjust();
    }
}


QString Page_Game_Farm::toString (const QString& pfx) const
{
    return "Page_Game_Farm {\n" +
            Page_Game::toString (pfx + "   ") + "\n" +
            pfx + QString("working: %1\n").arg(working) +
            pfx + QString("price: %1\n").arg(price) +
            pfx + QString("maxhours: %1\n").arg(maxhours) +
            pfx + QString("worktimer: %1\n").arg(worktimer.toString()) +
            pfx + "}";
}

//static
bool Page_Game_Farm::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Game_Farm");
    if (doc.findFirst("DIV[id=body] IMG.part_logo")
            .attribute("src").endsWith("/farm.jpg")) {
//        qDebug("Page_Game_Farm fits");
        return true;
    }
//    qDebug("Page_Game_Farm doesn't fits");
    return false;
}

bool Page_Game_Farm::doStartWork(int hours) {
    if (working) {
        qCritical("working flag already true");
        return false;
    }
    if (hours < 0 || hours > maxhours) {
        qCritical("hours out of range");
        return false;
    }
    foreach (QWebElement e, body.findAll("SELECT[id=work] OPTION")) {
        int n = e.attribute("value").trimmed().toInt();
        if (n == hours) {
            e.evaluateJavaScript("this.selected=true;");
            break;
        }
    }
    submit = body.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    qWarning("press submit");
    pressSubmit();
    return true;
}

bool Page_Game_Farm::doCancelWork() {
    if (!working) {
        qCritical("working flag already false");
        return false;
    }
    submit = body.findFirst("INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("submit not found");
        return false;
    }
    qWarning("press submit");
    pressSubmit();
    return true;
}
