#include <QRegExp>
#include <QWebElement>
#include <QWebElementCollection>
#include "page_game_post_messages.h"
#include "tools/tools.h"

Message::Message() {
    id  = 0;
    pit = QDateTime();
    td  = QWebElement();
}


Message::Message(const Message& that) {
    *this = that;
}


const Message& Message::operator=(const Message& that) {
    id  = that.id;
    pit = that.pit;
    td  = that.td;
    return *this;
}

bool Message::parse(QWebElement& tr) {
    QWebElementCollection tds = tr.findAll("TD");
    if (tds.count() != 3) {
        qDebug("bad tds.count: %d from %s",
               tds.count(), qPrintable(tr.toOuterXml()));
        return false;
    }
    id = tds[0].findFirst("INPUT").attribute("value").toInt();
    if (id <= 0) {
        qDebug("bad id: %d from %s", id, qPrintable(tr.toOuterXml()));
        return false;
    }
    //
    QRegExp rx("(\\d\\d\\.\\d\\d\\.\\d\\d \\d\\d:\\d\\d)");
    QString t = tds[1].toOuterXml();
    if (rx.indexIn(t) == -1) {
        qDebug("not fit to regexp from %s", qPrintable(tr.toOuterXml()));
        return false;
    }
    pit = QDateTime::fromString(rx.cap(1), "dd.MM.yy hh:mm");
    td = tds[2];
    return true;
}

QString Message::toString() const {
    QString s = td.toInnerXml();
    QRegExp rxB("<b \\s*[^>]*title=\"([^\"]+)\"[^>]*>");
    QRegExp rxX("<[^>]*>");
    QRegExp rxS("\\s+");
    s = s.replace(rxB, "[\\1]")
            .replace(rxX, " ")
            .replace("&nbsp;", " ")
            .replace(rxS, " ")
            .trimmed();
    return u8("{#%1 %2 :: %3}").arg(id).arg(::toString(pit)).arg(s);
}

Page_Game_Post_Messages::Page_Game_Post_Messages(QWebElement& doc) :
    Page_Game(doc) {
    pagekind = page_Game_Post_Messages;
    parse();
}


bool Page_Game_Post_Messages::fit(const QWebElement& doc) {
    if (doc.findFirst("TABLE.post_select").isNull()) {
        return false;
    }
    return true;
}


QString Page_Game_Post_Messages::toString (const QString& pfx) const {
    QString q;
    foreach (Message m, page_messages) {
        q += pfx + "   " + m.toString() + "\n";
    }

    return "Page_Game_Post_Messages {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + u8("%1 messages on page\n").arg(page_messages.count()) +
            q +
            pfx + "}\n";
}


void Page_Game_Post_Messages::parse() {
    page_messages.clear();
    foreach (QWebElement tr, document.findAll("TABLE.post_table TR")) {
        Message m;
        if (m.parse(tr)) page_messages.append(m);
    }

    foreach (QWebElement a, document.findAll("DIV.pages A")) {
    }
}

bool Page_Game_Post_Messages::doSelectFolder(int number) {
    return false;
}

bool Page_Game_Post_Messages::doSelectPage(int number) {
    return false;
}

bool Page_Game_Post_Messages::doSelectMessage(int id) {
    return false;
}

bool Page_Game_Post_Messages::doUnselectMessage(int id) {
    return false;
}

bool Page_Game_Post_Messages::doRemoveSelectedMessages() {
    return false;
}

bool Page_Game_Post_Messages::doRemoveAllMessages() {
    return false;
}
