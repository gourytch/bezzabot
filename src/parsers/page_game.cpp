#include <QWebElement>
#include <QWebElementCollection>
#include <QString>
#include <QDebug>
#include <QMap>
#include <QMapIterator>
#include "tools/tools.h"
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
    if (e.tagName () == "A" && e.attribute ("class") == "timer link")
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
// Page_Game
//
////////////////////////////////////////////////////////////////////////////

Page_Game::Page_Game (QWebElement& doc) :
    Page_Generic (doc)
{
    body = document.findFirst("DIV[id=body]");
    Q_ASSERT (!body.isNull());
    pagekind = page_Game;
    pagetitle= doc.findFirst("DIV[class=title]").toPlainText ().trimmed ();

    gold = -1;
    free_gold = -1;
    safe_gold = -1;
    {
        QString gStr = doc.findFirst ("DIV[id=gold]").attribute("onmouseover");
        //'total:|22517|;safe:|15360|;safe_add:|0|;pandora:|0|;free:|7157|'
        QRegExp rx ("'total:\\|(\\d+)\\|;safe:\\|(\\d+)\\|.*free:\\|(\\d+)\\|'");
        if (rx.indexIn(gStr) != -1) {
            gold        = dottedInt(rx.cap(1));
            safe_gold   = dottedInt(rx.cap(2));
            free_gold   = dottedInt(rx.cap(3));
        } else {
            gold = dottedInt (doc.findFirst ("DIV[id=gold] B").toInnerXml ());
            free_gold = gold;
        }
    }
    crystal = -1;
    free_crystal = -1;
    safe_crystal = -1;
    {
        QString cStr = doc.findFirst ("DIV[id=crystal]").attribute("onmouseover");
        QRegExp rx ("'total:\\|(\\d+)\\|;safe:\\|(\\d+)\\|.*free:\\|(\\d+)\\|'");
        if (rx.indexIn(cStr) != -1) {
            crystal         = dottedInt(rx.cap(1));
            safe_crystal    = dottedInt(rx.cap(2));
            free_crystal    = dottedInt(rx.cap(3));
        } else {
            crystal = dottedInt (doc.findFirst ("DIV[id=crystal] B").toInnerXml ());
            free_crystal = crystal;
        }
    }

    fish    = dottedInt (doc.findFirst ("DIV[id=fish] B").toInnerXml ());
    green   = dottedInt (doc.findFirst ("DIV[id=green] B").toInnerXml ());

    {
        QString hStr = doc.findFirst ("DIV[id=char]").attribute("onmouseover");
        QRegExp rx ("'now:\\|(\\d+)\\|;max:\\|(\\d+)\\|;speed:\\|(\\d+)\\|'");
        if (rx.indexIn(hStr) != -1)
        {
            hp_cur = dottedInt (rx.cap (1));
            hp_max = dottedInt (rx.cap (2));
            hp_spd = dottedInt (rx.cap (3));
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
            p.hp_cur = dottedInt (rx.cap (1));
            p.hp_max = dottedInt (rx.cap (2));
            p.hp_spd = dottedInt (rx.cap (3));
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
    chartitle = doc.findFirst("DIV[class=name] B").attribute ("title");
    charname = doc.findFirst("DIV[class=name] U").toPlainText ().trimmed ();
    message = doc.findFirst("DIV[class=message]").toPlainText ().trimmed ();
    workguild = WorkGuild_None;
    foreach (QWebElement e, doc.findFirst("DIV.guilds").findAll("A")) {
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
        } // FIXME добавить сюда обработку на все остальные гильдии
    }

    QWebElement e;
    e = doc.findFirst("B[id=time] SPAN");
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

    e = doc.findFirst("DIV[id=rmenu1] DIV[class=timers]");

    // 1: Таймер работы
    QWebElement c = e.firstChild();
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
    if (c.toPlainText().trimmed() == u8("Я в опасности!")) {
        timer_immunity.title = u8("DANGER");
        timer_immunity.href  = u8("");
        timer_immunity.pit   = QDateTime();
        timer_immunity.hms   = -1;
    } else {
        parseTimerSpan(c.findFirst("SPAN[class=js_timer]"),
                       &timer_immunity.pit, &timer_immunity.hms);
        timer_immunity.title = u8("SAFE");
        timer_immunity.adjust();
        timer_immunity.href  = u8("");
    }
    // 3. таймер бодалки
    c = c.nextSibling();
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
    QWebElement accordion = doc.findFirst("DIV[id=accordion]");
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
        QString t = c.toPlainText();//.trimmed();

//        if (r.id == 33 || r.id == 34) {
//            qDebug("*** inner=" + c.toInnerXml());
//            qDebug("*** text={" + t + "}");
//            QWebElement z = c.firstChild();
//            while (!z.isNull()) {
//                qDebug("*   z=" + z.toOuterXml());
//                z = z.nextSibling();
//            }
//            qDebug("*----*");
//        }

        r.count = t.toInt(&ok);
        if (!ok) {
            QRegExp rx( "(\\d+)\\s*/\\s*(\\d+)");
            if (rx.indexIn(c.toInnerXml()) == -1) {
                r.count = -1;
            } else {
                r.count = rx.cap(1).toInt(&ok);
                if (!ok) {
                    r.count = -1;
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
    e = accordion.findFirst("DIV.flyings");
    if (!e.isNull()) {
        FlyingInfo info;
        foreach (QWebElement div_title, e.findAll("DIV.title")) {
        // 1.title
            QWebElement a = div_title.firstChild();
            if (a.isNull() || a.tagName() != "A") continue;
            info.href = a.attribute("href");
            QWebElement b = a.firstChild();
            info.title = b.attribute("title");
            info.icon = b.attribute("class").split(" ")[1]; // FIXME
        // 2. content
            QWebElement div_content = div_title.nextSibling();
            if (div_content.tagName() != "DIV") continue;
            if (div_content.attribute("class") != "content") continue;
            info.init();
            if (info.title.startsWith(u8("Яйцо"))) {
                info.is_egg = true;
            } else {
                foreach (a, div_content.findAll("A")) {
                    QString text = e.toPlainText().trimmed().replace("%", "");
                    if (text == "Большое приключение") {
                        info.in_journey = true;
                        b = a.findFirst("SPAN.js_timer");
                        info.journey_cooldown.assign(b);
                    } else {
                        b = a.firstChild();
                        if (b.isNull()) continue;
                        QString title = b.attribute("title");
                        if (title == u8("Здоровье")) {
                            info.hits = text.toInt();
                        } else if (title == u8("Сытость зверушки")) {
                            info.feed = text.toInt();
                        } else if (title == u8("Золото")) {
                            info.gold = dottedInt(text);
                        }
                    }
                }
            }
            flyingslist.append(info);
        }
    }

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

QString toString(const FlyingInfo& info) {
    QString ret = u8("{%1, %2 state ").arg(info.title, info.icon);
    if (info.is_egg) {
        ret += "EGG";
    } else if (info.boxgame) {
        ret += u8("BOXGAME");
    } else if (info.in_journey) {
        ret += u8("JOURNEY till %1").arg(::toString(info.journey_cooldown.pit));
    } else {
        ret += u8("RESTING, feed=%1% hits=%2% gold=%3")
                .arg(info.feed)
                .arg(info.hits)
                .arg(info.gold);
    }
    ret += "}\n";
    return ret;
}

QString toString(const QString& pfx, const FlyingsList& list) {
    QString ret = "Flyings {\n";
    foreach(const FlyingInfo info, list) {
        ret += pfx + "   " + toString(info);
    }
    ret += pfx + "}\n";
    return ret;
}

QString Page_Game::toString (const QString& pfx) const
{

    return "Page_Game {\n" +
            pfx + Page_Generic::toString (pfx + "   ") + "\n" +
            pfx + "pagetitle: {" + pagetitle + "}\n" +
            pfx + "character: " + chartitle + " " + charname + "\n" +
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
                delay(7000 + (qrand() % 5000), false);
                qDebug("click on cage");
                actuate(a);
                delay(3000 + (qrand() % 5000), false);
                qDebug("clicked (i hope)");
                return closePopup();
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
    delay(3000 + (qrand() % 4000), false);
    qDebug("clicked. close popup");
    return closePopup();
}


bool Page_Game::closePopup() {
    QWebElement btn = document.findFirst("A.ui-dialog-titlebar-close");
    if (btn.isNull()) {
        qDebug("close button not found");
        return false;
    }
    qDebug("actuate close popup button");
    actuate(btn);
    delay(1000 + (qrand() % 2000), false);
    return true;
}
