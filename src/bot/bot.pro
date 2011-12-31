TOP = ../..

TARGET = bezzabot
TEMPLATE = app

QT += core gui network webkit

SOURCES += main.cpp\
        mainwindow.cpp \
        bot.cpp \
        webactor.cpp \
        persistentcookiejar.cpp \
        config.cpp \
        tunedpage.cpp \
        botthread.cpp

HEADERS  += mainwindow.h \
            bot.h \
            webactor.h \
            persistentcookiejar.h \
            config.h \
            tunedpage.h \
            botthread.h

INCLUDEPATH += $$TOP/src $$TOP/include
LIBS += -L$$TOP/lib -lparsers

OBJECTS_DIR = $$TOP/obj
DESTDIR = $$TOP/bin
