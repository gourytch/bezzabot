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
    _producer.registerClass<Page_Login>();
    _producer.registerClass<Page_Game_Index>();
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
    _producer.registerClass<Page_Game_Clan_Treasury>();
    _producer.registerClass<Page_Game>();
    _producer.registerClass<Page_Generic>();
}

void Parser::check() {
    if (!_initialized) {
        _initialized = true;
        setup();
    }
}

PageKind Parser::guessPageKind (const QWebElement& doc)
{
    QWebElementCollection divs = doc.findAll ("DIV[class=title]");
    QString logo_src = doc.findFirst ("IMG[class=part_logo]").attribute ("src");
    QString logo_name;
    if (! logo_src.isNull ())
    {
        QRegExp rx ("/([^/]+)\\.jpg$");
        if (rx.indexIn (logo_src))
        {
            logo_name = rx.cap (1);
        }
    }

    if (logo_name.isNull ())
    {
        qDebug () << "logo_name: isNull";
    }
    else
    {
        qDebug () << "logo_name:" << logo_name;
    }

    if (divs.count ())
    {
        QStringList titles;
        foreach (QWebElement e, divs)
        {
            titles.append (e.toPlainText ().trimmed ());
        }
        qDebug () << "Titles: {";
        foreach (QString e, titles)
        {
            qDebug () << "   {" << e << "}";
        }
        qDebug () << "}";
        QString t0 = titles [0];
        QString t1 = (titles.count () >= 2) ? titles [1] : "";

        if (t0 == u8 ("Мои подарки:"))
        {
            return page_Game_Index;
        }
        if (t0 == u8 ("Фермер Ушканчик"))
        {
            return page_Game_Farm;
        }
        if (t0 == u8 ("Шахтёр Геннадий (бывший оллигатор)"))
        {
            if (t1 == u8 ("Купильня"))
            {
                return page_Game_Mine_Main;
            }
            return page_Game_Mine_Shop;
        }
        if (logo_name == u8 ("Mine_Open"))
        {
            return page_Game_Mine_Open;
        }

        return page_Game;
    }
    else if (! doc.findFirst ("DIV[id=frontRight]").isNull())
    {
        qDebug () << "login detected";
        return page_Login;
    }
    else
    {
        qDebug () << "not indentified page";
        return page_Generic;
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
    return _producer.getObj(doc);
/*
    PageKind pagekind = guessPageKind (doc);
    switch (pagekind)
    {
    case page_Game_Index:
        return new Page_Game_Index (doc);

    case page_Game_Farm:
        return new Page_Game_Farm (doc);

    case page_Game_Mine_Open:
        return new Page_Game_Mine_Open (doc);

    case page_Game:
        return new Page_Game (doc);

    case page_Login:
        return new Page_Login (doc);

    case page_Generic:
    default:
        return new Page_Generic (doc);
    }
*/
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
