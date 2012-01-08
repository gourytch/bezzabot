TOP = ../..

CONFIG += debug console
QT += core gui network webkit

TARGET = surgery
TEMPLATE = app

SOURCES += \
    appwindow.cpp \
    surgery.cpp

HEADERS += \
    appwindow.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin




