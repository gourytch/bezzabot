#include <QtGui/QApplication>
#include "mainwindow.h"
#include "parsers/parser.h"

#define ORG_DOMAIN  "zhukoid.net"
#define ORG_NAME    "zhukoid"
#define APP_NAME    "qbezzabot"
#define APP_VERSION "0.0.1"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName (ORG_NAME);
    a.setOrganizationDomain (ORG_DOMAIN);
    a.setApplicationName (APP_NAME);
    a.setApplicationVersion (APP_VERSION);
///
    Parser::test ();
    return 0;
///

    MainWindow w;
    w.show();

    return a.exec();
}
