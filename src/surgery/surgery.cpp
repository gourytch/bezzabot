#include <iostream>
#include <QtGui/QApplication>

#include <QDir>
#include <QDebug>
#include <QString>
#include "appwindow.h"

using namespace std;

int main (int argc, char ** argv) {
    QApplication app(argc, argv);
    AppWindow w;
    w.show();
    return app.exec();
}
