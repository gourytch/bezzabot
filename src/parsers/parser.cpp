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

#include "tools.h"
#include "page_generic.h"
#include "page_game.h"
#include "page_game_index.h"
#include "page_game_farm.h"
#include "parser.h"



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

PageKind Parser::guessPageKind (const QWebElement& doc)
{
    if (doc.findFirst ("DIV[class=title]").toPlainText () == "Мои подарки:")
    {
        return page_Game_Index;
    }
    if (doc.findFirst ("DIV[class=title]").toPlainText () == "Фермер Ушканчик")
    {
        return page_Game_Farm;
    }
    if (doc.findFirst ("DIV[class=name]").isNull ())
    {
        return page_Generic;
    }
    else
    {
        return page_Game;
    }
}


Page_Generic* Parser::parse (const QString& text)
{
    QWebPage page;
    QWebElement doc = page.mainFrame()->documentElement ();
    doc.setInnerXml (text);
    if (doc.isNull ())
    {
        qDebug () << "SHIT HAPPERNS: EMPTY DOC";
        return NULL;
    }
    return parse (doc);
}


Page_Generic* Parser::parse (const QWebElement& doc)
{
    PageKind pagekind = guessPageKind (doc);
    switch (pagekind)
    {
    case page_Game_Index:
        return new Page_Game_Index (doc);

    case page_Game_Farm:
        return new Page_Game_Farm (doc);

    case page_Game:
        return new Page_Game (doc);

    case page_Generic:
    default:
        return new Page_Generic (doc);
    }
}


void Parser::test ()
{
    QDir dir;
    QStringList filters;
    filters << "sample-*-outer.xml";
    QStringList fnames = dir.entryList (filters);
    foreach (QString fname, fnames)
    {
        qDebug () << "LOAD " << fname;
        QString text = ::load (fname);
        Page_Generic *p = parse (text);
        if (p)
        {
            qDebug () << p->toString ("   ");
        }
    }
    qDebug () << "END";
}
