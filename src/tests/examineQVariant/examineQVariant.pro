CUR = $$PWD
TOP = $$CUR/../../..


QT       += core
QT       -= gui

TARGET = examineQVariant
CONFIG   += console debug
CONFIG   -= app_bundle

TEMPLATE = app
SOURCES += main.cpp

INCLUDEPATH += $$TOP/src
OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

