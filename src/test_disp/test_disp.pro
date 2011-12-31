TOP = ../..

QT += core

TARGET = test_disp
TEMPLATE = app

SOURCES += test_disp.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/bin

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers

