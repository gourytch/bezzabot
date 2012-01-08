TOP = ../..

QT += core gui network webkit

TARGET = test_parsers
TEMPLATE = app

SOURCES += test_parsers.cpp

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

