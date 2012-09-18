#include <QThread>
#include <QWebPage>
#include <QWebFrame>
#include <QWebElement>
#include <QWebElementCollection>
#include <QDateTime>
#include <QRegExp>
#include <QString>
#include <QStringList>
#include <QVector>
#include <QDebug>
#include <QDir>

#include "tools/tools.h"
#include "parser.h"
#include "all_pages.h"

static void dump_element (const QWebElement& e, const QString& adj)
{
    qDebug () << adj << "BEGIN " << e.tagName ();
    for (QWebElement s = e.firstChild ();
         ! s.isNull ();
         s = s.nextSibling ())
    {
        dump_element (s, adj + "  ");
    }
    qDebug () << adj << "END " << e.tagName ();
}


/////////////////////////////////////////////////////////////////////////
//
/////////////////////////////////////////////////////////////////////////

Parser::PageProducer Parser::_producer;

bool Parser::_initialized = false;

void Parser::setup() {
    _producer.registerClass<Page_Error>();
    _producer.registerClass<Page_UnderConstruction>();
    _producer.registerClass<Page_Login>();
    _producer.registerClass<Page_Game_Index>();
    _producer.registerClass<Page_Game_LevelUp>();
    _producer.registerClass<Page_Game_Dozor_Entrance>();
    _producer.registerClass<Page_Game_Dozor_LowHealth>();
    _producer.registerClass<Page_Game_Dozor_OnDuty>();
    _producer.registerClass<Page_Game_Dozor_GotVictim>();
    _producer.registerClass<Page_Game_Farm>();
    _producer.registerClass<Page_Game_Pier>();
    _producer.registerClass<Page_Game_Mine_Main>();
    _producer.registerClass<Page_Game_Mine_Open>();
    _producer.registerClass<Page_Game_Mine_LiveField>();
    _producer.registerClass<Page_Game_LuckySquare>();
    _producer.registerClass<Page_Game_Incubator>();
    _producer.registerClass<Page_Game_Clan_Treasury>();
    _producer.registerClass<Page_Game_Fight_Log>();
    _producer.registerClass<Page_Game_Post_Messages>();
    _producer.registerClass<Page_Game_Harbor_Market>();
    _producer.registerClass<Page_Game_Training>();
    _producer.registerClass<Page_Game_House_Plantation>();
    _producer.registerClass<Page_Game_Alchemy_Entrance>();
    _producer.registerClass<Page_Game_Alchemy_Lab>();
    _producer.registerClass<Page_Game_Grinder>();
    _producer.registerClass<Page_Game_Atlantis>();
    ///
    _producer.registerClass<Page_Game>();
    _producer.registerClass<Page_Generic>();
}

void Parser::check() {
    if (!_initialized) {
        _initialized = true;
        setup();
    }
}


Page_Generic* Parser::parse (const QString& text)
{
    QWebPage page;
    QWebElement doc = page.mainFrame()->documentElement ();
    doc.setInnerXml (text);
    if (doc.isNull ())
    {
        qDebug () << "SHIT HAPPENS: EMPTY DOC";
        return NULL;
    }
    return parse (doc);
}


Page_Generic* Parser::parse (QWebElement& doc)
{
    check();
    Page_Generic *p = _producer.getObj(doc);
//    if (p && getForegroundThread()) {
//        p->moveToThread(getForegroundThread());
//    }
    return p;
}


void Parser::test ()
{
    QDir dir ("../../bezzabot.samples");
    QStringList filters;
    filters << "*.xml";
    qDebug () << "parse " << dir.absolutePath () << filters [0];
    QStringList fnames = dir.entryList (filters);
    foreach (QString fname, fnames)
    {
        qDebug () << "LOAD " << fname;
        QString text = ::load (dir.absolutePath () + "/" + fname);
        Page_Generic *p = parse (text);
        if (p)
        {
            qDebug () << p->toString ("   ");
        }
    }
    qDebug () << "END";
}
