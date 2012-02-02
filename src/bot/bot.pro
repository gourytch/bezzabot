TOP = ../..

TARGET = bezzabot
TEMPLATE = app

CONFIG += debug
QT += core gui network webkit

win32 {
  CONFIG -= debug
  CONFIG += console
}

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    bot.cpp \
    webactor.cpp \
    farmersgroupsprices.cpp \
    bot_one_step.cpp \
    botstate.cpp \
    bot_got_page.cpp \
    work.cpp \
    workwatching.cpp \
    worksleeping.cpp \
    workmining.cpp \
    workfishing.cpp \
    workfieldsopening.cpp \
    workclangiving.cpp \
    workfarming.cpp \
    workscaryfighting.cpp

HEADERS  += \
    mainwindow.h \
    bot.h \
    webactor.h \
    farmersgroupsprices.h \
    botstate.h \
    work.h \
    workwatching.h \
    worksleeping.h \
    workmining.h \
    workfishing.h \
    workfieldsopening.h \
    workclangiving.h \
    workfarming.h \
    workscaryfighting.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

RESOURCES += \
    bot.qrc
































