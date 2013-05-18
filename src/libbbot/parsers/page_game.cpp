#include <QEventLoop>
#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include "tools/tools.h"
#include "types.h"
#include "page_game.h"

ESTART(WorkGuild)
ECASE(WorkGuild_None)
ECASE(WorkGuild_Miners)
ECASE(WorkGuild_Farmers)
ECASE(WorkGuild_Smiths)
ECASE(WorkGuild_Traders)
EEND

ESTART(PetKind)
ECASE(pet_Unknown)
ECASE(pet_Rat)
ECASE(pet_Cat)
ECASE(pet_Beawer)
ECASE(pet_Porcupines)
ECASE(pet_Racoon)
ECASE(pet_Armadillo)
ECASE(pet_Worm)
ECASE(pet_RedWorm)
ECASE(pet_Fox)
ECASE(pet_Unknown2)
ECASE(pet_Unknown3)
ECASE(pet_Monkey)
EEND

ESTART(FlyingKind)
ECASE(flying_Manticore)
ECASE(flying_Draco)
ECASE(flying_Grifan)
ECASE(flying_Ponny)
EEND

bool parseTimerSpan (const QWebElement& e, QDateTime *pit, int *hms)
{
    if (pit)
    {
        *pit = QDateTime ();
    }

    if (hms)
    {
        *hms = -1;
    }

    if (e.tagName () != "SPAN" ||
//        e.attribute ("class") != "js_timer" ||
        e.attribute ("timer").isNull())
    {
        return false;
    }

    QString tstr = e.attribute ("timer");

    QRegExp rx ("^(\\d*)\\|(\\d*)");
    if (rx.indexIn (tstr) == -1)
    {
        return false;
    }
    QString pitStr = rx.cap (1);
    if (pitStr.isEmpty()) {
        pitStr = "0";
    }
    bool ok;
    int pitInt = pitStr.toInt (&ok);
    if (ok)
    {
        if (pitInt == 0) {
            *pit = QDateTime(); // NULL TIME
        } else {
            pit->setTime_t (pitInt);
        }
    }
    tstr = e.toInnerXml(); //.firstChild ().toPlainText ();
    rx.setPattern ("(\\d+):(\\d\\d):(\\d\\d):(\\d\\d)");
    if (rx.indexIn (tstr) != -1)
    {
        QString d = rx.cap (1);
        QString h = rx.cap (2);
        QString m = rx.cap (3);
        QString s = rx.cap (4);
        if (hms)
        {
            *hms = s.toInt () +
                    m.toInt () * 60 +
                    h.toInt () * 3600 +
                    d.toInt() * 86400;
        }
    }
    else
    {
        rx.setPattern ("(\\d\\d):(\\d\\d):(\\d\\d)");
        if (rx.indexIn (tstr) == -1)
        {
            return false;
        }
        QString h = rx.cap (1);
        QString m = rx.cap (2);
        QString s = rx.cap (3);
        if (hms)
        {
            *hms = s.toInt () + m.toInt () * 60 + h.toInt () * 3600;
        }
    }
    return true;
}


int PageTimer::systime_delta = 0;

QString PageTimer::toString () const
{
    return QString("Timer {pit=%1, hms=%2, href={%3}, title={%4}}")
            .arg(pit.toString ("yyyy-MM-dd HH:mm:ss"),
                 QString::number(hms), href, title);
}


const PageTimer& PageTimer::operator= (const PageTimer &v)
{
    title   = v.title;
    href    = v.href;
    pit     = v.pit;
    hms     = v.hms;
    return *this;
}


void PageTimer::assign (const QWebElement &e)
{
    if (e.tagName () == "A" && e.attribute ("class").contains("timer"))
    {
        title = "?";
        href = e.attribute ("href");
        parseTimerSpan (e.findFirst ("SPAN"), &pit, &hms);
    }
    else if (e.tagName() == "LI")
    {
        title = e.attribute("title");
        href = e.findFirst("A").attribute("href");
        parseTimerSpan (e.findFirst ("SPAN"), &pit, &hms);
    } else if (e.tagName() == "SPAN") {
        title = "";
        href = "";
        parseTimerSpan (e, &pit, &hms);
    }
    adjust();
}

void PageTimer::adjust() {
    if (pit.isNull()) {
        return;
    }
    pit = pit.addSecs(systime_delta);
}

const PageTimer& PageTimers::operator [] (int ix) const
{
    return timers.at (ix);
}

QString PageTimers::toString (const QString& pfx) const
{
    QString buf = "PageTimers {\n";
    for (int i = 0; i < count (); i++)
    {
        buf += pfx + "   " + timers.at (i).toString () + "\n";
    }
    buf += pfx + "}";
    return buf;
}

const PageTimer* PageTimers::byTitle(const QString& title) const {
    TimersIterator i(timers);
    while (i.hasNext()) {
        if (i.peekNext().title.indexOf(title) >= 0) {
            return &(i.peekNext());
        }
        i.next();
    }
    return NULL;
}

bool PageCoulon::assign(const QWebElement& e) {
// <a item_id="15371239" title="Скороход 99/100"><b class="item_98"></b></a>
    if (e.tagName() != "A") {
        return false;
    }

    QString title = e.attribute("title");
    if (title.isNull()) {
        return false;
    }

    bool ok;
    id = e.attribute("item_id").toInt(&ok);
    if (!ok) {
        return false;
    }

    active = e.attribute("class") == "active";

    QRegExp rx ("(.*) (\\d+)/(\\d+)");
    if (rx.indexIn(title) != -1) {
        name = rx.cap(1).trimmed();
        cur_lvl = rx.cap(2).toInt(&ok);
        if (!ok) {
            return false;
        }
        max_lvl = rx.cap(3).toInt(&ok);
        if (!ok) {
            return false;
        }
    }
    kind = e.firstChild().attribute("class");
    return true;
}

QString PageCoulon::toString(const QString& pfx) const {
    return pfx + u8("{id=%1, kind=%2, name={%3} lvl=%4 of %5, %6}")
            .arg(id)
            .arg(kind)
            .arg(name)
            .arg(cur_lvl)
            .arg(max_lvl)
            .arg(active ? "active" : "inactive");
}

bool PageCoulons::assign(const QWebElement& e) {
    clear();
    foreach (QWebElement a, e.findAll("A")) {
        PageCoulon c;
        if (c.assign(a)) {
            coulons.append(c);
        }
    }
    return true;
}

QString PageCoulons::toString(const QString& pfx) const {
    QString ret = u8("PageCoulons {\n");
    for (int i = 0; i < coulons.size(); ++i) {
        ret += pfx + "   " + coulons[i].toString() + "\n";
    }
    return ret + pfx + "}\n";
}

const PageCoulon* PageCoulons::byId(quint32 id) const {
    for (int i = 0; i < coulons.size(); ++i) {
        if (coulons[i].id == id) {
            return &coulons[i];
        }
    }
    return NULL;
}

const PageCoulon* PageCoulons::byName(const QString& name) const {
    for (int i = 0; i < coulons.size(); ++i) {
        if (coulons[i].name == name) {
            return &coulons[i];
        }
    }
    return NULL;
}

QString PageCoulons::stringById(quint32 id) const {
    if (id == NULL_COULON) return "NULL_COULON";
    const PageCoulon* coulon = byId(id);
    return coulon ? coulon->toString() : "NONEXISTENT_COULON";
}

const PageCoulon* PageCoulons::active() const {
    for (int i = 0; i < coulons.size(); ++i) {
        if (coulons[i].active) {
            return &coulons[i];
        }
    }
    return NULL;
}

////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////

bool FlyingInfo::Caption::parse(QWebElement &div_title) {
    valid = false;
    QWebElement e = div_title.findFirst("A");
    if (e.isNull()) return false;
    actionObj = e;
    href = e.attribute("href");
    e = e.firstChild();
    if (e.tagName() != "B") return false;
    title = e.attribute("title");
    QRegExp rx(".*(flying_\\d+)");
    if (rx.indexIn(e.attribute("class")) == -1) return false;
    icon = rx.cap(1);
    valid = true;
    return true;
}


QString FlyingInfo::Caption::toString() const {
    if (!valid) {
        return "??invalid??";
    }
    return u8("{icon=\"%1\" title=\"%2\"}").arg(icon, title);
}


bool FlyingInfo::Egg::parse(QWebElement &content) {
    valid = false;
    QWebElement e = content.firstChild();
    if (e.tagName() != "SPAN") return false;
    QRegExp rx(u8("Состояние:(\\d+)%"));
    if (rx.indexIn(e.toInnerXml()) == -1) return false;
    condition = rx.cap(1).toInt();
    e = e.nextSibling();
    if (e.tagName() != "FORM") return false;
    e = e.nextSibling();
    if (e.tagName() != "DIV") return false;
    e = e.nextSibling();
    if (e.tagName() != "SPAN") return false;
    rx = QRegExp(u8("Чистка:\\s*(\\d)\\s*/\\s*(\\d)"));
    if (rx.indexIn(e.toInnerXml()) == -1) return false;
    cleanings_performed = rx.cap(1).toInt();
    cleanings_total = rx.cap(2).toInt();
    e = e.nextSibling();
    if (e.tagName() == "I") {
        e = e.findFirst("SPAN.js_timer");
        if (e.isNull()) return false;
        cleanings_cooldown.assign(e);
        valid = true;
        return true;
    } else if (e.tagName() == "FORM") {
        cleanings_cooldown = PageTimer();
        valid = true;
        return true;
    }
    return false;
}


bool FlyingInfo::Egg::canClean() const {
    if (cleanings_performed == cleanings_total) return false;
    if (cleanings_cooldown.active()) return false;
    return true;
}


QString FlyingInfo::Egg::toString() const {
    if (!valid) {
        return "??invalid??";
    }
    return u8("{cond:%1% clean:%2/%3 %4}")
            .arg(condition)
            .arg(cleanings_performed)
            .arg(cleanings_total)
            .arg(cleanings_cooldown.toString());
}


bool FlyingInfo::Normal::parse(QWebElement &content) {
    valid = false;
    QWebElementCollection anchors = content.findAll("A");
    if (anchors.count() != 3) return false;
    feed_url    = anchors[0].attribute("href");
    heal_url    = anchors[1].attribute("href");
    train_url   = anchors[2].attribute("href");
    QRegExp rx(">\\s*(\\d+)%\\s*<");
    if (rx.indexIn(anchors[0].toOuterXml()) == -1) return false;
    feed = dottedInt(rx.cap(1), NULL);
    if (feed < 0 || feed > 100) {
        qCritical(u8("bad food level: %1; xml text: {%2}, cap: {%3}")
                  .arg(feed)
                  .arg(anchors[0].toOuterXml())
                  .arg(rx.cap(1)));
    }
    if (rx.indexIn(anchors[1].toOuterXml()) == -1) return false;
    hits = rx.cap(1).trimmed().toInt();
    rx = QRegExp(">\\s*([0123456789.]+)\\s*<");
    if (rx.indexIn(anchors[2].toOuterXml()) == -1) return false;
    gold = dottedInt(rx.cap(1).trimmed(), NULL);
    valid = true;
    return true;
}


QString FlyingInfo::Normal::toString() const {
    if (!valid) {
        return "??invalid??";
    }
    return u8("{feed=%1% hits=%2% gold=%3}").arg(feed).arg(hits).arg(gold);
}


bool FlyingInfo::Journey::parse(QWebElement &content) {
    valid = false;
    QWebElement e = content.findFirst("CENTER B A");
    if (e.isNull()) return false;
    QString xml = e.toOuterXml();
    if (xml.contains(u8(">Кар Кар<"))) {
        title = "приключение Кар Кар";
    } else {
        QRegExp rx(u8(">((Большое|Маленькое) приключение)<"));
        if (rx.indexIn(xml) == -1) return false;
        title = rx.cap(1);
    }
    e = content.findFirst("SPAN[timer]");
    if (e.isNull()) return false;
    journey_cooldown.assign(e);
    valid = true;
    return true;
}


QString FlyingInfo::Journey::toString() const {
    if (!valid) return "??invalid??";
    return u8("{journey:\"%1\", cooldown:%2}")
            .arg(title)
            .arg(journey_cooldown.toString());
}


bool FlyingInfo::Boxgame::parse(QWebElement &content) {
    valid = false;
    QWebElement e = content.findFirst("FORM INPUT[type=submit]");
    if (e.isNull()) return false;
    if (e.attribute("value") != u8("ПОМОЧЬ")) return false;
    valid = true;
    return true;
}


QString FlyingInfo::Boxgame::toString() const {
    if (!valid) return "??invalid??";
    return u8("{boxgame}");
}


bool FlyingInfo::Attacked::parse(QWebElement &content) {
    valid = false;
//    qDebug(u8("FlyingInfo::Attacked::parse(%1)").arg(content.toOuterXml()));
    QWebElement e = content.findFirst("CENTER B A");
    if (e.isNull()) return false;
    QString s = e.toInnerXml();
    if (! s.contains(u8("Нападение на летуна!"))) {
//        qDebug(u8("{%1}").arg(s));
        return false;
    }
    valid = true;
    return true;
}

QString FlyingInfo::Attacked::toString() const {
    if (!valid) return "??invalid??";
    return u8("{attacked}");
}


FlyingInfo::FlyingInfo() {
    init();
}


//FlyingInfo::FlyingInfo(const FlyingInfo& that) {
//    *this = that;
//}


void FlyingInfo::init() {
    valid   = false;
    caption = Caption();
    egg     = Egg();
    normal  = Normal();
    journey = Journey();
    boxgame = Boxgame();
}


//const FlyingInfo& FlyingInfo::operator=(const FlyingInfo& that) {
//    valid   = that.valid;
//    caption = that.caption;
//    egg     = that.egg;
//    normal  = that.normal;
//    journey = that.journey;
//    boxgame = that.boxgame;

//    return *this;
//}


bool FlyingInfo::parse(QWebElement& element) {
    init();
    if (element.tagName() != "DIV" || element.attribute("class") != "title")
        return false;
    if (!caption.parse(element)) return false;
    QWebElement e = element.nextSibling();
    if (e.tagName() != "DIV" || e.attribute("class") != "content")
        return false;
    valid = egg.parse(e) ||
            normal.parse(e) ||
            journey.parse(e) ||
            boxgame.parse(e) ||
            attacked.parse(e);
    return valid;
}


QString FlyingInfo::toString() const {
    if (!valid) return "?INVALID FLYING?";
    QString s = caption.toString();
    if (egg.valid)      s += egg.toString();
    if (normal.valid)   s += normal.toString();
    if (journey.valid)  s += journey.toString();
    if (boxgame.valid)  s += boxgame.toString();
    if (attacked.valid) s += attacked.toString();
    return "{" + s + "}";
}

typedef QMap<QString, int> ValMap;
ValMap parseSafeText(QString s) {
    QRegExp rx("([^:|]+):\\|(\\d+)\\|;?");
//    rx.setMinimal(true);
    ValMap r;
    int offs = s.indexOf("','");
    if (offs == -1) return r;
    offs += 3;
    while ((offs = rx.indexIn(s, offs)) != -1) {
        r.insert(rx.cap(1), dottedInt(rx.cap(2), NULL));
        offs += rx.matchedLength();
    }
    return r;
}


////////////////////////////////////////////////////////////////////////////
//
// Page_Game
//
////////////////////////////////////////////////////////////////////////////

Page_Game::Page_Game (QWebElement& doc) :
    Page_Generic (doc) {
    parse();
}

bool Page_Game::parse() {
    body = document.findFirst("DIV[id=body]");
    Q_ASSERT (!body.isNull());
    pagekind = page_Game;
    foreach (QWebElement e, document.findAll("DIV.title")) {
        if (isDisplayed(e)) {
            pagetitle = e.toPlainText().trimmed();
            break;
        }
    }
//  pagetitle= doc.findFirst("DIV[class=title]").toPlainText ().trimmed ();

    gold = -1;
    free_gold = -1;
    safe_gold = -1;
    {
        QString gStr = document.findFirst ("DIV[id=gold]").attribute("onmouseover");
        //'total:|22517|;safe:|15360|;safe_add:|0|;pandora:|0|;free:|7157|'
        ValMap m = parseSafeText(gStr);
        if (m.size() > 0) {
            gold        = m.value("total", -1);
            safe_gold   = m.value("safe", -1);
            free_gold   = m.value("free", -1);
        } else {
            gold = dottedInt (document.findFirst ("DIV[id=gold] B").toInnerXml (), NULL);
            free_gold = gold;
        }
    }
    crystal = -1;
    free_crystal = -1;
    safe_crystal = -1;
    {
        QString cStr = document.findFirst ("DIV[id=crystal]").attribute("onmouseover");
        ValMap m = parseSafeText(cStr);
        if (m.size() > 0) {
            crystal         = m.value("total", -1);
            safe_crystal    = m.value("safe", -1);
            free_crystal    = m.value("free", -1);
        } else {
            crystal = dottedInt (document.findFirst ("DIV[id=crystal] B").toInnerXml (), NULL);
            free_crystal = crystal;
        }
    }

    fish    = dottedInt (document.findFirst ("DIV[id=fish] B").toInnerXml (), NULL);
    green   = dottedInt (document.findFirst ("DIV[id=green] B").toInnerXml (), NULL);

    {
        QString hStr = document.findFirst ("DIV[id=char]").attribute("onmouseover");
        QRegExp rx ("'now:\\|(\\d+)\\|;max:\\|(\\d+)\\|;speed:\\|(\\d+)\\|'");
        if (rx.indexIn(hStr) != -1)
        {
            hp_cur = dottedInt (rx.cap (1), NULL);
            hp_max = dottedInt (rx.cap (2), NULL);
            hp_spd = dottedInt (rx.cap (3), NULL);
        }
        QRegExp rx_cage("'(uncage|cage)',\\s*(\\d+)");

        foreach (QWebElement e, document.findAll("DIV#pet")) {
            QWebElement s = e.firstChild();
            PetInfo p;

            if (s.tagName() != "A") {
                qDebug("??? FIRST SIBLING IS NOT ANCHOR: " + s.toOuterXml());
                continue;
            }
            QString href = s.attribute("href");
            if (rx_cage.indexIn(href) == -1) {
                qDebug(QString("??? BAD HREF {%1} FORMAT FOR {%2}")
                       .arg(href).arg(s.toOuterXml()));
                continue;
            }
            p.active = (rx_cage.cap(1) == "cage");
            p.id = rx_cage.cap(2).toInt();

            s = s.nextSibling();
            if (s.tagName() != "DIV") {
                qDebug("??? SECOND SIBLING IS NOT DIV: " + s.toOuterXml());
                continue;
            }
            if (rx.indexIn(s.attribute("onmouseover")) == -1) {
                qDebug("??? BAD ONMOUSEOVER FORMAT FOR " + s.toOuterXml());
                continue;
            }
            p.hp_cur = dottedInt (rx.cap (1), NULL);
            p.hp_max = dottedInt (rx.cap (2), NULL);
            p.hp_spd = dottedInt (rx.cap (3), NULL);
            s = s.findFirst("B");
            if (!s.isNull()) {
                QRegExp rx("pet(\\d+)");
                if (rx.indexIn(s.attribute("class")) > -1) {
                    p.kind = (PetKind)rx.cap(1).toInt();
                }
            }
            petlist.append(p);
        }
    }
    chartitle = document.findFirst("DIV[class=name] B").attribute ("title");
    charname = document.findFirst("DIV[class=name] U").toPlainText ().trimmed ();
    foreach (QWebElement e, document.findAll("DIV.message")) {
        if (isDisplayed(e)) {
            message = e.toPlainText().trimmed();
//            qDebug("[message] displayed: {%s}", qPrintable(message));
            break;
        } else {
//            qDebug("[message] hidden: {%s}",
//                   qPrintable(e.toPlainText().trimmed()));
        }
    }
//    message = doc.findFirst("DIV[class=message]").toPlainText ().trimmed ();
    workguild = WorkGuild_None;
    alchemicalguild = false;
    foreach (QWebElement e, document.findFirst("DIV.guilds").findAll("A")) {
        QString title = e.attribute("title");
        if (title == u8("Шахтеры")) {
            workguild = WorkGuild_Miners;
        } else if (title == u8("Работяги")) {
            workguild = WorkGuild_Farmers;
        } else if (title == u8("Железячники")) {
            workguild = WorkGuild_Smiths;
        } else if (title == u8("Толстосумы")) {
            workguild = WorkGuild_Traders;
//      } else if (title == u8("Стражики")) {
//      } else if (title == u8("Бандюки")) {
//      } else if (title == u8("Мирники")) {
//      } else if (title == u8("Устрашатели")) {
//      } else if (title == u8("Пернатый спецназ")) {
//      } else if (title == u8("Краснокожие")) {
//      } else if (title == u8("Клыкуны")) {
//      } else if (title == u8("Теневоды")) {
        } else if (title == u8("Братство Алхимиков")) {
            alchemicalguild = true;
        } // FIXME добавить сюда обработку на все остальные гильдии
    }

    QWebElement e;
    e = document.findFirst("B[id=time] SPAN");
    timer_system.title = "SYSTEM TIME";
    timer_system.href = "/";
    parseTimerSpan(e, &timer_system.pit, &timer_system.hms);
    if (!timer_system.pit.isNull()) {
        QDateTime dayStart = QDateTime(QDate::currentDate());
        QDateTime sysStart = timer_system.pit;
        QDateTime now = QDateTime::currentDateTime();
        int tloc = dayStart.secsTo(now);
        int tsys = timer_system.hms;
        int thrs = sysStart.secsTo(dayStart);
        PageTimer::systime_delta = thrs + tloc - tsys;
/*
        qDebug(QString("daystart = %1 = %2")
               .arg(dayStart.toString("yyyy-MM-dd hh:mm:ss"))
               .arg(dayStart.toTime_t()));
        qDebug(QString("sysstart = %1 = %2")
               .arg(sysStart.toString("yyyy-MM-dd hh:mm:ss"))
               .arg(sysStart.toTime_t()));
        qDebug("tloc = %d", tloc);
        qDebug("tsys = %d", tsys);
        qDebug("thrs = %d", thrs);
*/
        qDebug("systime_delta = %d sec", PageTimer::systime_delta);
    }

    e = document.findFirst("DIV[id=rmenu1] DIV[class=timers]");

    // 1: Таймер работы
    QWebElement c = e.firstChild();
//    qDebug("first duplet: work timer");
//    qDebug(" #1: " + c.toOuterXml());
    if (c.toPlainText().trimmed() == u8("Я свободен!")) {
        timer_work.title = u8("FREE");
        timer_work.href  = u8("/");
        timer_work.pit   = QDateTime();
        timer_work.hms   = -1;
    } else {
        timer_work.assign(c);
        timer_work.title = (timer_work.hms == 0) ? "READY": "BUSY";
    }
    // 2. таймер иммунитета
    c = c.nextSibling();
//    qDebug("second duplet: immunity timer");
//    qDebug(" #1: " + c.toOuterXml());
    if (c.toPlainText().trimmed() == u8("Я в опасности!")) {
        timer_immunity.title = u8("DANGER");
        timer_immunity.href  = u8("");
        timer_immunity.pit   = QDateTime();
        timer_immunity.hms   = -1;
    } else {
        QWebElement t = c.findFirst("SPAN.js_timer");
//        qDebug("immtimer=" + t.toOuterXml());
        parseTimerSpan(t,
                       &timer_immunity.pit, &timer_immunity.hms);
        timer_immunity.title = u8("SAFE");
        timer_immunity.adjust();
        timer_immunity.href  = u8("");
    }
    // 3. таймер бодалки
    c = c.nextSibling();
//    qDebug("third duplet: battle timer");
//    qDebug(" #1: " + c.toOuterXml());
    if (c.toPlainText().trimmed() == u8("Пора в бой!")) {
        timer_attack.title = u8("BATTLETIME");
        timer_attack.href  = u8("dozor.php");
        timer_attack.pit   = QDateTime();
        timer_attack.hms   = -1;
    } else {
        parseTimerSpan(c.findFirst("SPAN[class=js_timer]"),
                       &timer_attack.pit, &timer_attack.hms);
        timer_attack.adjust();
        timer_attack.title = (timer_attack.hms == 0) ? "READY": "RESTING";
        timer_attack.href  = u8("");
    }

    timers.clear();
    QWebElement accordion = document.findFirst("DIV[id=accordion]");
    e = accordion.findFirst("DIV[class=counters]");
    foreach (c, e.findAll("LI")) {
        timers.add(c);
    }

    e = accordion.findFirst("DIV[class=resources]");
    resources.clear();
    foreach (c, e.findAll("LI")) {
        PageResource r;
        r.id = c.attribute("id").mid(1).toInt();
        r.title = c.attribute("title");
        QWebElement a = c.findFirst("A");
        r.href = a.attribute("href");
        bool ok;
        QString t = c.toPlainText().trimmed();

//        if (r.id == 56)
//        {
//            qDebug(QString("*** rid=%1").arg(r.id));
//            qDebug("*** inner=" + c.toInnerXml());
//            qDebug("*** text={" + t + "}");
//            QWebElement z = c.firstChild();
//            while (!z.isNull()) {
//                qDebug("*   z=" + z.toOuterXml());
//                z = z.nextSibling();
//            }
//            qDebug("*----*");
//        }

        r.count = dottedInt(t, &ok);
        if (!ok) {
            QRegExp rx( "(\\d+)\\s*/\\s*(\\d+)");
            if (rx.indexIn(c.toInnerXml()) == -1) {
                r.count = -1;
                // дерьмовый toPlainText
                QRegExp rx2("</a>\\s*([0123456789+-.]+)\\s*</li>");
                if (rx2.indexIn(c.toOuterXml())) {
//                    qDebug(u8("cap={%1}").arg(rx2.cap(1)));
                    r.count = dottedInt(rx2.cap(1), &ok);
                } else {
                    qDebug(u8("unparseable#1 t={%1} from c={%2}")
                       .arg(t, c.toOuterXml()));
                }
            } else { // nnn/mmm
                r.count = dottedInt(rx.cap(1), &ok);
                if (!ok) {
                    r.count = -1;
                    qDebug(u8("unparseable#2 t={%1} from c={%2}")
                           .arg(t, c.toOuterXml()));
                }
            }
        }
        resources[r.id] = r;
    }

    effects.clear();
    e = accordion.findFirst("DIV[class=timers]");
    foreach (c, e.findAll("LI")) {
        effects.add(c);
    }

    coulons.assign(document.findFirst("DIV[id=coulons_bar]"));

    // летучки
    parseFlyingList();

    parseMedikit();

    return true;
}


bool Page_Game::parseFlyingList() {
    flyingslist.clear();
    QWebElement e = document.findFirst("DIV#accordion DIV.flyings");
    if (e.isNull()) return false;
    FlyingInfo info;
    foreach (QWebElement div_title, e.findAll("DIV.title")) {
        info.init();
        if (info.parse(div_title)) flyingslist.append(info);
    }
    return true;
}

QString toString(const QString& pfx, const PageResource& r) {
    return pfx + QString("{id=%1, count=%2, href=%3, title=%4}")
            .arg(r.id).arg(r.count).arg(r.href).arg(r.title);
}

QString toString(const QString& pfx, const PageResources& s) {
    QString ret = "Resources: {\n";
    for (PageResources::ConstIterator i = s.constBegin(); i != s.constEnd(); ++i) {
        ret += ::toString(pfx + "   ", *i) + "\n";
    }
    ret += pfx +"}\n";
    return ret;
}

QString toString(const QString& pfx, const PetInfo& p) {
    QString ret = QString("{id=%1, hp_cur/hp_max/hp_spd=%2/%3/%4, kind=%5 title={%6}, %7}")
            .arg(p.id)
            .arg(p.hp_cur).arg(p.hp_max).arg(p.hp_spd)
            .arg(::toString(p.kind))
            .arg(p.title)
            .arg(p.active ? "uncaged" : "caged");
    return pfx + ret;
}

QString toString(const QString& pfx, const PetList& petlist) {
    QString ret = "Pet List: {\n";
    for (PetList::ConstIterator i = petlist.constBegin(); i != petlist.constEnd(); ++i) {
        ret += ::toString(pfx + "   ", *i) + "\n";
    }
    ret += pfx +"}\n";
    return ret;
}

QString toString(const QString& pfx, const FlyingsList& list) {
    QString ret = "Flyings {\n";
    foreach(const FlyingInfo info, list) {
        ret += pfx + "   " + info.toString() + "\n";
    }
    ret += pfx + "}\n";
    return ret;
}

QString Page_Game::toString (const QString& pfx) const
{

    return "Page_Game {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + u8("pagetitle: {%1}\n").arg(pagetitle) +
            pfx + u8("character: %1 %2\n").arg(chartitle, charname) +
            pfx + QString("hp: %1/%2, spd %3\n")
            .arg(hp_cur).arg(hp_max).arg(hp_spd) +
            pfx + QString("gold: %1, crystal: %2, fish:%3, green:%4\n")
            .arg(gold).arg(crystal).arg(fish).arg(green) +
            pfx + QString("safe_gold:%1, safe_crystal:%2\n")
            .arg(safe_gold).arg(safe_crystal) +
            pfx + QString("free_gold:%1, free_crystal:%2\n")
            .arg(free_gold).arg(free_crystal) +
            pfx + QString("workquild: %1\n").arg(::toString(workguild)) +
            pfx + QString("messsage: {%1}\n").arg(message) +
            pfx + QString("system timer: %1\n").arg(timer_system.toString()) +
            pfx + QString("work   timer: %1\n").arg(timer_work.toString()) +
            pfx + QString("immun. timer: %1\n").arg(timer_immunity.toString()) +
            pfx + QString("fight  timer: %1\n").arg(timer_attack.toString()) +
            pfx + QString("other timers:\n") +
            pfx + timers.toString (pfx + "   ") + "\n" +
            pfx + QString("resources:\n") +
            pfx + ::toString(pfx + "   ", resources) + "\n" +
            pfx + QString("effects:\n") +
            pfx + effects.toString(pfx + "   ") + "\n" +
            pfx + QString("coulons:\n") +
            pfx + coulons.toString(pfx + "   ") + "\n" +
            pfx + QString("pets:\n") +
            pfx + ::toString(pfx + "   ", petlist) +
            pfx + QString("flyings:\n") +
            pfx + ::toString(pfx + "   ", flyingslist) +
            pfx + medikitToString() + "\n" +
            pfx + "}";
}

//static
bool Page_Game::fit(const QWebElement& doc) {
//    qDebug("* CHECK Page_Game");

    if (doc.findFirst("DIV[id=char]").isNull()) {
//        qDebug("Page_Game not fit: has no char");
        return false;
    }

    if (doc.findFirst("DIV.top_money").isNull()) {
//        qDebug("Page_Game not fit: has no top_money");
        return false;
    }

    if (doc.findFirst("DIV[id=menu]").isNull()) {
//        qDebug("Page_Game not fit: has no menu");
        return false;
    }

    if (doc.findFirst("DIV[id=body]").isNull()) {
//        qDebug("Page_Game not fit: has no body");
        return false;
    }

    if (doc.findFirst("DIV[id=rmenu1]").isNull()) {
//        qDebug("Page_Game not fit: has no rmenu1");
        return false;
    }

    if (doc.findFirst("DIV[id=rmenu2]").isNull()) {
//        qDebug("Page_Game not fit: has no rmenu2");
        return false;
    }

//    qDebug("Page_Game fit");
    return true;
}

bool Page_Game::hasNoJob() const {
    return (timer_work.hms == -1);
}

QString Page_Game::jobLink(bool ifFinished, int timegap) const {
    if (timer_work.pit.isNull()) {
        return QString();
    }

    if (ifFinished) {
        QDateTime ts = QDateTime::currentDateTime();
        if (timegap > 0) {
            if (ts < timer_work.pit.addSecs(timegap)) {
                return QString();
            }
        }
    }
    return timer_work.href;
}

bool Page_Game::doClickOnCoulon(quint32 id) {
//  "/ajax.php?m=coulon&item=15371239"
    foreach (QWebElement a, document.findAll("DIV.coulons A")) {
        if ((quint32)(a.attribute("item_id").toInt()) == id) {
            delay(500 + (qrand() % 3000), true);
            qDebug(QString("actuate coulon #%1 (%2)")
                   .arg(id).arg(a.attribute("title")));
            actuate(a);
            return true;
            break;
        }
    }
    qCritical(QString("coulon #%1 not found").arg(id));
    return false;
}

bool Page_Game::uncagePet(int id) {
    QRegExp rx("'(cage|uncage)', (\\d+)");
    foreach (QWebElement a, document.findAll("DIV#pet A.ico_cage")) {
        if (rx.indexIn(a.attribute("href")) > -1) {
            if (rx.cap(1) == "cage") {
                qCritical("pet id=%d already uncaged", id);
                return false;
            } else {
                qDebug("found cage with pet id=%d, uncage it", id);
                delay(500 + (qrand() % 3000), false);
                qDebug("click on cage");
                actuate(a);
                if (waitForPopup()) {
                    qDebug("got popup. close it");
                    return closePopup();
                } else {
                    qDebug("popup is missing :(");
                    return false;
                }
            }
        }
    }
    qCritical("cage with pet id=%d not found", id);
    return false;
}


bool Page_Game::cagePet() {
    QRegExp rx("'(cage|uncage)',\\s*(\\d+)");
    QWebElement a = document.findFirst("DIV#pet A.ico_cage");
    if (a.isNull()) {
        qCritical("pet div>a not found at all");
        return false;
    }
    if (rx.indexIn(a.attribute("href")) == -1) {
        qCritical("something strange with pet div>a: %s",
                  qPrintable(a.toOuterXml()));
        return false;
    }
    if (rx.cap(1) == "uncage") {
        qCritical("pet already caged");
        return false;
    }
    qDebug("found uncaged pet id=%d, save it", rx.cap(2).toInt());
    delay(700 + (qrand() % 2000), false);
    qDebug("click on cage");
    actuate(a);
    if (waitForPopup()) {
        int ms = 700 + (qrand() % 2000);
        qDebug("got popup. wait %d ms then close popup", ms);
        delay(ms, false);
        return closePopup();
    } else {
        qDebug("popup is missing :(");
        return false;
    }
}


bool Page_Game::activateJSUpdateWatcher(bool force) {
    if (force) {
        deactivateJSUpdateWatcher();
    } else {
        if (jsWatcherActivated) {
            return true;
        }
    }

    injectJSInsiders();
    connect(this, SIGNAL(js_doUpdateInfo_finished()),
            this, SLOT(slot_catch_js_update()));
    jsWatcherActivated = true;
    jsUpdateFinished = false;
    return true;
}


bool Page_Game::deactivateJSUpdateWatcher() {
    disconnect(this, SIGNAL(js_doUpdateInfo_finished()),
               this, SLOT(slot_catch_js_update()));
    jsWatcherActivated = false;
    return true;
}


bool Page_Game::waitForJSUpdate(int ms) {
    if (ms <= 0) {
        ms = randrange(1000, 10000);
    }
    qDebug("wait for page update");
    QTime t;
    QEventLoop loop;
    while (!jsUpdateFinished && (t.elapsed() < ms)) {
        loop.processEvents();
    }
    if (jsUpdateFinished) {
        qDebug("got update after %d ms", t.elapsed());
    } else {
        qDebug("timeout after %d ms", t.elapsed());
    }
    return true;
}

void Page_Game::slot_catch_js_update() {
    jsUpdateFinished = true;
}


bool Page_Game::parseMedikit() {
    medikitOpened = false;
    numGreenPotions = -1;
    numBluePotions = -1;
    numRedPotions = -1;
    QRegExp rx(">(\\d+)<");

    QWebElement medikit = document.findFirst("TABLE#potions_popup");
    if (medikit.isNull()) {
        return true;
    }

    QWebElement e = medikit.findFirst("TD#potion_td_1 SPAN.count_amount");
    if (e.isNull()) {
        qCritical("cannot find potion_td_1");
        return false;
    }
    if (rx.indexIn(e.toOuterXml()) == -1) {
        qCritical("cannot fit span " + e.toOuterXml());
        return false;
    }
    numGreenPotions = rx.cap(1).toInt();

    e = medikit.findFirst("TD#potion_td_2 SPAN.count_amount");
    if (e.isNull()) {
        qCritical("cannot find potion_td_2");
        return false;
    }
    if (rx.indexIn(e.toOuterXml()) == -1) {
        qCritical("cannot fit span " + e.toOuterXml());
        return false;
    }
    numBluePotions = rx.cap(1).toInt();

    e = medikit.findFirst("TD#potion_td_3 SPAN.count_amount");
    if (e.isNull()) {
        qCritical("cannot find potion_td_3");
        return false;
    }
    if (rx.indexIn(e.toOuterXml()) == -1) {
        qCritical("cannot fit span " + e.toOuterXml());
        return false;
    }
    numRedPotions = rx.cap(1).toInt();

    medikitOpened = true;
    return true;
}

bool Page_Game::doOpenMedikit() {
    QWebElement e = document.findFirst("A.potion_all");
    if (e.isNull()) {
        qCritical("medikit opener not found");
        return false;
    }
    qDebug("open medikit");
    actuate(e);
    if (!waitForPopup()) {
        qCritical("medikit not opened");
    }
    qDebug("... medikit opened");
    return true;
}

bool Page_Game::doCloseMedikit() {
    qDebug("close medikit");
    if (!closePopup()) {
        qCritical("medikit not closed");
        return false;
    }
    qDebug("medikit closed");
    return true;
}

bool Page_Game::doDrinkPotion(MedikitPotion potion) {
    QString id;
    switch (potion) {
    case Potion_Green:
        id = "potion_td_1";
        break;
    case Potion_Blue:
        id = "potion_td_2";
        break;
    case Potion_Red:
        id = "potion_td_3";
        break;
    default:
        qCritical("unknown potion id: %d", (int)potion);
        return false;
    }
    QWebElement e = document.findFirst("TD#" + id + " A.cmd_all");
    if (e.isNull()) {
        qCritical("not found " + id);
        return false;
    }
    if (e.toPlainText() != u8("ВЫПИТЬ")) {
        qCritical(u8("button text = {%1}").arg(e.toPlainText()));
        return false;
    }
    qDebug(u8("actuate button on id=%1").arg(id));
    injectJSInsiders();
    actuate(e);
    activateJSUpdateWatcher();
    connect(this, SIGNAL(js_doUpdateInfo_finished()),
            this, SLOT(slot_update_finished()));
    waitForJSUpdate();
    deactivateJSUpdateWatcher();
    return true;
}

bool Page_Game::doBuyOnePotion(MedikitPotion /*potion*/) {
    return false;
}

bool Page_Game::doBuyAllPotion(MedikitPotion /*potion*/) {
    return false;
}

QString Page_Game::medikitToString() const {
    if (!medikitOpened) {
        return "medikit closed";
    }
    QString s;
    s.sprintf("potions in medikit: %d green, %d blue, %d red",
             numGreenPotions, numBluePotions, numRedPotions);
    return s;
}


bool Page_Game::waitForPopup(int ms) {
    qDebug("awaiting for popup with %d ms timeout", ms);
    return wait4("A.ui-dialog-titlebar-close", true, ms);
}

bool Page_Game::waitForPopupClosed(int ms) {
    return wait4("A.ui-dialog-titlebar-close", false, ms);
}

bool Page_Game::closePopup() {
    // btn надо удалить перед ожиданием
    QWebElement btn = document.findFirst("A.ui-dialog-titlebar-close");
    if (btn.isNull()) {
        qDebug("close button not found");
        return true;
    }
    QString id = btn.attribute("id");
    if (id.isEmpty()) {
        id = "__close_popup__";
        btn.setAttribute("id", id);
    }
    qDebug("actuate close popup button by id {%s}", qPrintable(id));
    actuate(id);
    return waitForPopupClosed();
}

bool Page_Game::doShowFlyingsAccordion() {
    QWebElement accordion = document.findFirst("DIV#accordion");
    if (accordion.isNull()) {
        qCritical("accordion not found");
        return false;
    }
    QWebElement e = accordion.findFirst("DIV.flyings");
    if (e.isNull()) {
        qCritical("panel flyings not found");
        return false;
    }
    if (isDisplayed(e)) {
        qDebug("panel with flyings is already visible");
        return true;
    }
    qDebug("panel with flyings is not visible");
    foreach (QWebElement tab, accordion.findAll("H3[role=tab]")) {
        if (tab.toPlainText().contains(u8("Инкубатор"))) {
            qDebug("found tab, actuate it");
            actuate(tab);
            if (!wait4("DIV#accordion DIV.flyings")) {
                qDebug("fail :(");
                return false;
            }
            delay(333 + (qrand() % 777), false);
            qDebug("seems now is ok");
            return true;
        }
    }
    qDebug("tab not found :(");
    return false;
}


bool Page_Game::doFlyingBoxgame(int flyingNo) {
    if (flyingslist.size() < flyingNo) {
        qCritical("flyings pos too big");
        return false;
    }
    if (!flyingslist.at(flyingNo).boxgame.valid) {
        qCritical("boxgame is not active");
        return false;
    }
    if (!doShowFlyingsAccordion()) {
        qCritical("accordion tab not visible");
        return false;
    }
    submit = document.findFirst("DIV#accordion DIV.flyings")
            .findAll("DIV.content")[flyingNo]
            .findFirst("FORM INPUT[type=submit]");
    if (submit.isNull()) {
        qCritical("boxbutton missing");
        return false;
    }
    qDebug("submitting boxgame");
    pressSubmit();
    return true;
}

bool Page_Game::doFlyingGoEvents(int flyingNo) {
    if (flyingslist.size() < flyingNo) {
        qCritical("flyings pos %d is too big", flyingNo);
        return false;
    }
    const FlyingInfo & fi = flyingslist.at(flyingNo);
    qDebug("... flying info: " + fi.toString());
    if (!(fi.normal.valid || fi.boxgame.valid || fi.journey.valid)) {
        qCritical("normal/boxgame/journey is not active for flyingNo=%d", flyingNo);
        return false;
    }
    if (!doShowFlyingsAccordion()) {
        qCritical("accordion tab not visible");
        return false;
    }

    QWebElementCollection all = document.findFirst("DIV#accordion DIV.flyings")
            .findAll("DIV.title");
    if (all.count() != flyingslist.size()) {
        qFatal("titles=%d, flyinglist=%d", all.count(), flyingslist.count());
        return false;
    }
    submit = all[flyingNo].findFirst("A");
//    submit = fi.caption.actionObj;
    if (submit.isNull()) {
        qCritical("link missing for flyingNo=%d", flyingNo);
        return false;
    }
    qDebug(u8("submitting eventlink for flyingNo=%1 (%2)")
           .arg(flyingNo).arg(fi.caption.title));
    qDebug("submit=" + submit.toInnerXml());
    pressSubmit();
    return true;
}


bool Page_Game::doLookAtAttackResults(int flyingNo) {
    if (flyingslist.size() < flyingNo) {
        qCritical("flyings pos %d is too big", flyingNo);
        return false;
    }
    const FlyingInfo & fi = flyingslist.at(flyingNo);
    qDebug("... flying info: " + fi.toString());
    if (!(fi.attacked.valid)) {
        qCritical("flyingNo=%d is not in attacked state", flyingNo);
        return false;
    }
    if (!doShowFlyingsAccordion()) {
        qCritical("accordion tab not visible");
        return false;
    }

    QWebElementCollection all = document.findFirst("DIV#accordion DIV.flyings")
            .findAll("DIV.title");
    if (all.count() != flyingslist.size()) {
        qFatal("titles=%d, flyinglist=%d", all.count(), flyingslist.count());
        return false;
    }
    submit = all[flyingNo].nextSibling().findFirst("FORM INPUT[type=submit]");
    if (submit.isNull()) {
        qFatal("SUBMIT BUTTON NOT FOUND");
    }
    qDebug(u8("submitting look-at-attack-results button for flyingNo=%1 (%2)")
           .arg(flyingNo).arg(fi.caption.title));
    qDebug("submit=" + submit.toInnerXml());
    pressSubmit();
    return true;
}

bool Page_Game::isJSInjected() {
    bool hasPageObj   = document.evaluateJavaScript("typeof _page_ != 'undefined';").toBool();
    bool hasOldUpdate = document.evaluateJavaScript("typeof old_doUpdateInfo != 'undefined';").toBool();
    qDebug(u8("check js-injection for %1").arg(::toString(pagekind)));
    if (hasPageObj) {
        qDebug("already injected: has page");
        if (hasOldUpdate) {
            qDebug("... and old update-function");
        } else {
            qDebug("... ??? but has no old update-function");
        }
        return true;
    }
    qDebug("not injected yet");
    return false;
}

void Page_Game::injectJSInsiders() {
    if (isJSInjected()) return;
    qDebug("injectJSInsiders into Page(%p)", this);

    connect(this, SIGNAL(js_injected()), this, SLOT(slot_js_injected()));
//    connect(this, SIGNAL(js_doUpdateInfo_invoked(QString,QString)), this, SLOT(slot_update_invoked(QString,QString)));
//    connect(this, SIGNAL(js_doUpdateInfo_finished()), this, SLOT(slot_update_finished()));

    webframe->addToJavaScriptWindowObject("_page_", this);
    QString js = webframe->evaluateJavaScript("doUpdateInfo.toString()").toString();
    js = js.replace("doUpdateInfo", "old_doUpdateInfo");
    webframe->evaluateJavaScript(js);

    webframe->evaluateJavaScript(
                "function serialize(_obj) {"
                "  if (typeof _obj === 'undefined') {"
                "    return 'undefined';"
                "  }"
                "  if (typeof _obj.toSource !== 'undefined' && typeof _obj.callee === 'undefined') {"
                "    return _obj.toSource();"
                "  }"
                "  switch (typeof _obj) {"
                "    case 'number':"
                "    case 'boolean':"
                "    case 'function':"
                "      return _obj;"
                "      break;"
                "    case 'string':"
                "      return '\\'' + _obj + '\\'';"
                "      break;"
                "    case 'object':"
                "      var str;"
                "      if (_obj.constructor === Array || typeof _obj.callee !== 'undefined') {"
                "        str = '[';"
                "        var i, len = _obj.length;"
                "        for (i = 0; i < len-1; i++) { str += serialize(_obj[i]) + ','; }"
                "        str += serialize(_obj[i]) + ']';"
                "      } else {"
                "        str = '{';"
                "        var key;"
                "        for (key in _obj) { str += key + ':' + serialize(_obj[key]) + ','; }"
                "        str = str.replace(/\\,$/, '') + '}';"
                "      }"
                "      return str;"
                "      break;"
                "    default:"
                "      return 'UNKNOWN';"
                "      break;"
                "  }"
                "}"
                "function doUpdateInfo(data, config) {\n"
                "  _page_.js_doUpdateInfo_invoked(serialize(data), serialize(config));\n"
                "  old_doUpdateInfo(data, config);\n"
                "  _page_.js_doUpdateInfo_finished();\n"
                "};\n"
                "setTimeout(function() {_page_.js_injected();}, 500);\n"
                );
    QString txt;
    txt = webframe->evaluateJavaScript("old_doUpdateInfo.toString();").toString();
//    qDebug(u8("old_doUpdateInfo:"));
//    qDebug(txt);
    txt = webframe->evaluateJavaScript("doUpdateInfo.toString();").toString();
//    qDebug(u8("new doUpdateInfo:"));
//    qDebug(txt);
}


void Page_Game::slot_js_injected() {
    qDebug("js_injection checked, this=%p, thrid=%p",
           this, (void*)QThread::currentThreadId());
}

void Page_Game::slot_update_invoked(QString data, QString config) {
    qDebug(u8("doUpdateInfo ('%1', '%2') {").arg(data, config));
}

void Page_Game::slot_update_finished() {
    qDebug("}// doUpdateInfo");

}
