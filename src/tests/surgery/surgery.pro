CUR = $$PWD
TOP = $$CUR/../../..

CONFIG += debug
QT += core gui network webkit

TARGET = surgery
TEMPLATE = app

SOURCES += \
    appwindow.cpp \
    surgery.cpp \
    alertdialog.cpp

HEADERS += \
    appwindow.h \
    alertdialog.h

RESOURCES += \
    surgery.qrc

FORMS += \
    alertdialog.ui

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools
PRE_TARGETDEPS += $$TOP/lib/libtools.a $$TOP/lib/libparsers.a

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin




