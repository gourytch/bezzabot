#include <iostream>
#include <QtGui/QApplication>
#include <QDir>
#include <QDebug>
#include <QString>
#include <QWebElement>
#include <QWebElementCollection>
#include "parsers/all_pages.h"
#include "tools/tools.h"
#include "tools/config.h"
#include "parsers/parser.h"
#include "parsers/all_pages.h"

using namespace std;

int main (int argc, char ** argv) {
    QApplication dummy(argc, argv);
    QString samples = Config::appDirPath() + "/../../bezzabot.samples/";
    QString fname = samples + "not_a_pig-20120108_101354-outer.xml";
    cout << "loading page ..." << endl;
    QString text = ::load (fname);
    cout << "parsing page ..." << endl;
    Page_Generic *pGeneric = Parser::parse(text);
    cout << "check generic ..." << endl;
    Q_CHECK_PTR(pGeneric);
    cout << "dyncast to page_game ..." << endl;
    Page_Game *p = dynamic_cast<Page_Game*>(pGeneric);
    cout << "check page_game ..." << endl;
    Q_CHECK_PTR(p);
    cout << "check document ..." << endl;
    Q_ASSERT(!(p->document.isNull()));
    quint32 id = 15371239;
    cout << "search for anchor #" << id << "..." << endl;
//    foreach (QWebElement a, p->document.findFirst("DIV.coulons").findAll("A")) {
    foreach (QWebElement a, p->document.findAll("DIV.coulons A")) {
        cout << "check anchor ..." << endl;
        Q_ASSERT(!a.isNull());
        if (a.attribute("item_id").toInt() == id) {
            cout << "FOUND!" << endl;
            cout << qPrintable(a.toOuterXml()) << "}" << endl;
            break;
        }
    }
    cout << "OK" << endl;
    return 0;
}
