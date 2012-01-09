#include <iostream>
#include <QtGui/QApplication>

#include <QDir>
#include <QDebug>
#include <QString>
#include "appwindow.h"
#include "tools/logger.h"
using namespace std;

int main (int argc, char ** argv) {
    QApplication app(argc, argv);
    Logger::global();
    qDebug("initialize window");
    AppWindow w;
    w.show();
    qDebug("execute application");
    return app.exec();
}
