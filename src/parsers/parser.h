#ifndef PARSERS_PARSER_H
#define PARSERS_PARSER_H

#include <QString>
#include <QWebElement>
#include "types.h"
#include "page_generic.h"

class Parser
{
public:
    static PageKind guessPageKind (const QWebElement& doc);

    static Page_Generic* parse (const QString& text);

    static Page_Generic* parse (const QWebElement& doc);

    static void test ();


};


#endif // PARSER_H
