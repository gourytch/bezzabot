#include <QtGui/QApplication>
#include <QDir>
#include <QDebug>
#include <QString>
#include "parsers/all_pages.h"
#include "tools/tools.h"
#include "tools/config.h"
#include "parsers/parser.h"


int main (int argc, char ** argv) {
    QApplication dummy(argc, argv);
    QDir dir (Config::appDirPath() + "../../../bezzabot.samples");
    QStringList filters;
    filters << "*.xml";
    dir = QDir(dir.absolutePath ());
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
