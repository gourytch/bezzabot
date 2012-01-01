#include <iostream>
#include <QtGui/QApplication>
#include <QDir>
#include <QDebug>
#include <QString>
#include "parsers/all_pages.h"
#include "tools/tools.h"
#include "tools/config.h"
#include "parsers/parser.h"

using namespace std;

int main (int argc, char ** argv) {
    QApplication dummy(argc, argv);
    QDir dir (Config::appDirPath() + "../../../bezzabot.samples");
    QStringList filters;
    filters << "*.xml";
    dir = QDir(dir.absolutePath ());
    cout << "parse " << qPrintable(dir.absolutePath () + filters [0]) << endl;
    QStringList fnames = dir.entryList (filters);
    foreach (QString fname, fnames)
    {
        cout << "LOAD " << qPrintable(fname) << endl;
        QString text = ::load (dir.absolutePath () + "/" + fname);
        Page_Generic *p = Parser::parse (text);
        if (p) {
            cout << qPrintable(p->toString ("   ")) << endl;
        } else {
            cout << "UNPARSEABLE" << endl;
        }
        cout << endl;
        cout << "---------------------------------------------------" << endl;
        cout << endl;
        cout << endl;
    }
    cout << "END" << endl;
    return 0;
}
