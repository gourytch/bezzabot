#include <QtGui/QApplication>
#include "mainwindow.h"
#include "config.h"
#include "parsers/parser.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setOrganizationName (ORG_NAME);
//    a.setOrganizationDomain (ORG_DOMAIN);
//    a.setApplicationName (APP_NAME);
//    a.setApplicationVersion (APP_VERSION);

#if 1
///
//    Config::test ();
    Parser::test ();
    return 0;
///
#else
    MainWindow w;
    w.show();
    return a.exec();
#endif
}
