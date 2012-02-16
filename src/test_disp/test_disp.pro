TOP = ../..

CONFIG += debug console
QT += core

TARGET = test_disp
TEMPLATE = app

SOURCES += test_disp.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -ltools -lparsers
PRE_TARGETDEPS += $$TOP/lib/libtools.a $$TOP/lib/libparsers.a

