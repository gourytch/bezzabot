TOP = ../..

TARGET = bezzabot
TEMPLATE = app

QT += core gui network webkit

SOURCES += main.cpp\
        mainwindow.cpp \
        bot.cpp \
        webactor.cpp \
        botthread.cpp

HEADERS  += mainwindow.h \
            bot.h \
            webactor.h \
            botthread.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/bin
