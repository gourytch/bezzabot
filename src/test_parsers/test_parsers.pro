TOP = ../..

QT += core gui network webkit

TARGET = test_parsers
TEMPLATE = app

SOURCES += test_parsers.cpp

INCLUDEPATH += $$TOP/src $$TOP/include
LIB += -L$$TOP/lib -lparsers

OBJECTS_DIR = $$TOP/obj
DESTDIR = $$TOP/bin

