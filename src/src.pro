#-------------------------------------------------
#
# Project created by QtCreator 2011-08-18T23:56:20
#
#-------------------------------------------------

QT       += core gui network webkit

TARGET = bezzabot
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    bot.cpp \
    webactor.cpp \
    persistentcookiejar.cpp \
    tools.cpp \
    config.cpp \
    tunedpage.cpp \
    botthread.cpp

HEADERS  += mainwindow.h \
    bot.h \
    webactor.h \
    persistentcookiejar.h \
    tools.h \
    config.h \
    tunedpage.h \
    botthread.h

include (parsers/parsers.pri)

FORMS    +=

OBJECTS_DIR = ../obj
DESTDIR = ../bin








