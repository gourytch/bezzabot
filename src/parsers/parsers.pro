TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt debug warn_on staticlib
QT     += core webkit

TOP = ../..

HEADERS += \
    types.h \
    page_generic.h \
    page_login.h \
    page_game.h \
    page_game_index.h \
    page_game_farm.h \
    page_game_mine_open.h \
    all_pages.h \
    parser.h \
    page_game_dozor_entrance.h \
    page_game_dozor_onduty.h \
    page_game_dozor_gotvictim.h

SOURCES += \
    types.cpp \
    page_generic.cpp \
    page_login.cpp \
    page_game.cpp \
    page_game_index.cpp \
    page_game_farm.cpp \
    page_game_mine_open.cpp \
    parser.cpp \
    page_game_dozor_entrance.cpp \
    page_game_dozor_onduty.cpp \
    page_game_dozor_gotvictim.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/lib

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -lparsers -ltools









