TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt debug warn_on staticlib
QT     += core network webkit

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
    page_game_dozor_gotvictim.h \
    page_game_dozor_lowhealth.h \
    page_game_pier.h \
    page_error.h \
    page_game_mine_main.h \
    page_game_mine_livefield.h \
    page_game_luckysquare.h \
    page_game_clan_treasury.h \
    page_game_incubator.h \
    page_game_fight_log.h \
    page_underconstruction.h \
    page_game_levelup.h \
    page_game_post_messages.h \
    page_game_harbor_market.h \
    page_game_training.h \
    page_game_house_plantation.h \
    page_game_alchemy_entrance.h \
    page_game_alchemy_lab.h

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
    page_game_dozor_gotvictim.cpp \
    page_game_dozor_lowhealth.cpp \
    page_game_pier.cpp \
    page_error.cpp \
    page_game_mine_main.cpp \
    page_game_mine_livefield.cpp \
    page_game_luckysquare.cpp \
    page_game_clan_treasury.cpp \
    page_game_incubator.cpp \
    page_game_fight_log.cpp \
    page_underconstruction.cpp \
    page_game_levelup.cpp \
    page_game_post_messages.cpp \
    page_game_harbor_market.cpp \
    page_game_training.cpp \
    page_game_house_plantation.cpp \
    page_game_alchemy_entrance.cpp \
    page_game_alchemy_lab.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/lib

INCLUDEPATH += $$TOP/src
LIBS += -L$$TOP/lib -ltools
PRE_TARGETDEPS += $$TOP/lib/libtools.a

win32 {
  CONFIG -= debug
  CONFIG += console
}



















































