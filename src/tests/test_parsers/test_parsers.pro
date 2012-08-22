CUR = $$PWD
TOP = $$CUR/../../..

CONFIG += qt debug warn_on
QT += core gui network webkit

TARGET = test_parsers
TEMPLATE = app

SOURCES += test_parsers.cpp

INCLUDEPATH += $$TOP/src/libbbot
LIBS += -L$$TOP/lib -lbbot
PRE_TARGETDEPS += $$TOP/lib/libbbot.a

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin
