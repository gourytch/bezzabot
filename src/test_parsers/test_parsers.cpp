#include <QtGui/QApplication>
#include "parsers/all_pages.h"
#include "tools/tools.h"
#include "parsers/parser.h"
#include <QDir>
#include <QDebug>
#include <QString>


int main (int argc, char ** argv) {
    QApplication dummy(argc, argv);
    Parser::test();


    QDir dir ("../../bezzabot.samples");
    QStringList filters;
    filters << "*.xml";
    qDebug () << "parse " << dir.absolutePath () << filters [0];
    QStringList fnames = dir.entryList (filters);
    foreach (QString fname, fnames)
    {
        qDebug () << "LOAD " << fname;
        QString text = ::load (dir.absolutePath () + "/" + fname);
        Page_Generic *p = Parser::parse (text);
        if (p)
        {
            qDebug () << p->toString ("   ");
        }
    }
    qDebug () << "END";


    return 0;
}
