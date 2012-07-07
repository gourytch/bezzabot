#ifndef PARSERS_PARSER_H
#define PARSERS_PARSER_H

#include <QString>
#include <QWebElement>
#include "types.h"
#include "page_generic.h"
#include "tools/producer.h"

class Parser
{
protected:
    typedef Producer<Page_Generic, QWebElement> PageProducer;

    static PageProducer _producer;

    static bool _initialized;

    static void check();

    static void setup();

public:

    static Page_Generic* parse (const QString& text);

    static Page_Generic* parse (QWebElement& doc);

    static void test ();


};


#endif // PARSER_H
