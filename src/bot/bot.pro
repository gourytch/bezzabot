TOP = ../..

TARGET = bezzabot
TEMPLATE = app

QT += core gui network webkit

SOURCES += main.cpp\
        mainwindow.cpp \
        bot.cpp \
        webactor.cpp \
        botthread.cpp \
    bot_handle_Page_Game_Dozor_Entrance.cpp \
    bot_handle_Page_Game_Mine_Open.cpp \
    farmersgroupsprices.cpp \
    bot_handle_Page_Game_Pier.cpp

HEADERS  += mainwindow.h \
            bot.h \
            webactor.h \
            botthread.h \
    farmersgroupsprices.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/bin

RESOURCES += \
    bot.qrc





