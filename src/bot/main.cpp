#include <QtGui/QApplication>
#include "mainwindow.h"
#include "tools/tools.h"
#include "tools/config.h"
#include "parsers/parser.h"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    setForegroundThread();
//    a.setOrganizationName (ORG_NAME);
//    a.setOrganizationDomain (ORG_DOMAIN);
//    a.setApplicationName (APP_NAME);
//    a.setApplicationVersion (APP_VERSION);
    qsrand(QDateTime::currentDateTime().toTime_t());
    MainWindow *wnd = MainWindow::getInstance();
    wnd->show();
    return a.exec();
}
