TOP = ../..

TARGET = bezzabot
TEMPLATE = app

CONFIG += debug
QT += core gui network webkit

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
    worksleeping.cpp

HEADERS  += \
    mainwindow.h \
    bot.h \
    webactor.h \
    botthread.h \
    farmersgroupsprices.h \
    botstate.h \
    work.h \
    workwatching.h \
    worksleeping.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

RESOURCES += \
    bot.qrc








