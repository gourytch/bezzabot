TOP = ../..

TARGET = bezzabot
TEMPLATE = app

CONFIG += debug
QT += core gui network webkit

SOURCES += main.cpp\
        mainwindow.cpp \
        bot.cpp \
        webactor.cpp \
        botthread.cpp \
    bot_handle_Page_Game_Dozor_Entrance.cpp \
    bot_handle_Page_Game_Mine_Open.cpp \
    farmersgroupsprices.cpp \
    bot_handle_Page_Game_Pier.cpp \
    bot_handle_Page_Game_Farm.cpp \
    bot_one_step.cpp

HEADERS  += mainwindow.h \
            bot.h \
            webactor.h \
            botthread.h \
    farmersgroupsprices.h

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

RESOURCES += \
    bot.qrc

win32 {
    CONFIG += static
    SOURCES += static_plugins.cpp
    QTPLUGIN += \
        qjpeg qgif qico qsvg qtiff \
        qdecorationdefault qdecorationwindows
}
