CUR = $$PWD
TOP = $$CUR/../../..

QT       += network webkit testlib

TARGET = tst_treemaptest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

SOURCES += tst_treemaptest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -ltools
PRE_TARGETDEPS += $$TOP/lib/libtools.a

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin
