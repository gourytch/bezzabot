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

INCLUDEPATH += $$TOP/src/libbbot
LIBS += -L$$TOP/lib -lbbot
PRE_TARGETDEPS += $$TOP/lib/libbbot.a

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin




