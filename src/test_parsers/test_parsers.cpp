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

void test(const QString& fname) {
    cout << "LOAD " << qPrintable(fname) << endl;
    QString text = ::load (fname);
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

int main (int argc, char ** argv) {
    QApplication dummy(argc, argv);
    Config::global();

    test(Config::appDirPath() + "/TEST.xml");
    return 0;

    if (argc == 1) {
        QDir dir (Config::appDirPath() + "../../../bezzabot.samples");
        QStringList filters;
        filters << "*.xml";
        dir = QDir(dir.absolutePath ());
        cout << "parse " << qPrintable(dir.absolutePath () + filters [0]) << endl;
        QStringList fnames = dir.entryList (filters);
        foreach (QString fname, fnames)
        {
            test(dir.absolutePath () + "/" + fname);
        }
    } else {
        for (int i = 1; i < argc; ++i) {
            QString fname = argv[i];
            test(fname);
        }
    }
    cout << "END" << endl;
    return 0;
}
