TOP = ../..

CONFIG += debug console
QT += core gui network webkit

TARGET = surgery
TEMPLATE = app

SOURCES += surgery.cpp

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/bin

