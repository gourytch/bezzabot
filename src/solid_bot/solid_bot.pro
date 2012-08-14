TOP = $$PWD/../..
SRC = $$TOP/src

TARGET = solid_bot
TEMPLATE = app

CONFIG += debug
QT += core gui network webkit

win32 {
  CONFIG -= debug
  CONFIG += console
}

INCLUDEPATH += $$SRC


SOURCES += \
    $$SRC/tools/tools.cpp \
    $$SRC/tools/config.cpp \
    $$SRC/tools/tunedpage.cpp \
    $$SRC/tools/persistentcookiejar.cpp \
    $$SRC/tools/logger.cpp \
    $$SRC/tools/timebomb.cpp \
    $$SRC/tools/netmanager.cpp \
    $$SRC/tools/activityhours.cpp \
    $$SRC/tools/tarball.cpp \
    $$SRC/tools/varmap.cpp \
    $$SRC/tools/treemap.cpp \
    $$SRC/tools/vartools.cpp

SOURCES += \
    $$SRC/parsers/types.cpp \
    $$SRC/parsers/page_generic.cpp \
    $$SRC/parsers/page_login.cpp \
    $$SRC/parsers/page_game.cpp \
    $$SRC/parsers/page_game_index.cpp \
    $$SRC/parsers/page_game_farm.cpp \
    $$SRC/parsers/page_game_mine_open.cpp \
    $$SRC/parsers/parser.cpp \
    $$SRC/parsers/page_game_dozor_entrance.cpp \
    $$SRC/parsers/page_game_dozor_onduty.cpp \
    $$SRC/parsers/page_game_dozor_gotvictim.cpp \
    $$SRC/parsers/page_game_dozor_lowhealth.cpp \
    $$SRC/parsers/page_game_pier.cpp \
    $$SRC/parsers/page_error.cpp \
    $$SRC/parsers/page_game_mine_main.cpp \
    $$SRC/parsers/page_game_mine_livefield.cpp \
    $$SRC/parsers/page_game_luckysquare.cpp \
    $$SRC/parsers/page_game_clan_treasury.cpp \
    $$SRC/parsers/page_game_incubator.cpp \
    $$SRC/parsers/page_game_fight_log.cpp \
    $$SRC/parsers/page_underconstruction.cpp \
    $$SRC/parsers/page_game_levelup.cpp \
    $$SRC/parsers/page_game_post_messages.cpp \
    $$SRC/parsers/page_game_harbor_market.cpp \
    $$SRC/parsers/page_game_training.cpp \
    $$SRC/parsers/page_game_house_plantation.cpp \
    $$SRC/parsers/page_game_alchemy_entrance.cpp \
    $$SRC/parsers/page_game_alchemy_lab.cpp \
    $$SRC/parsers/page_game_grinder.cpp

SOURCES += \
    $$SRC/bot/main.cpp\
    $$SRC/bot/mainwindow.cpp \
    $$SRC/bot/alertdialog.cpp \
    $$SRC/bot/bot.cpp \
    $$SRC/bot/webactor.cpp \
    $$SRC/bot/farmersgroupsprices.cpp \
    $$SRC/bot/bot_one_step.cpp \
    $$SRC/bot/botstate.cpp \
    $$SRC/bot/bot_got_page.cpp \
    $$SRC/bot/work.cpp \
    $$SRC/bot/workwatching.cpp \
    $$SRC/bot/worksleeping.cpp \
    $$SRC/bot/workmining.cpp \
    $$SRC/bot/workfishing.cpp \
    $$SRC/bot/workfieldsopening.cpp \
    $$SRC/bot/workclangiving.cpp \
    $$SRC/bot/workfarming.cpp \
    $$SRC/bot/workscaryfighting.cpp \
    $$SRC/bot/workflyingbreeding.cpp \
    $$SRC/bot/worktraining.cpp \
    $$SRC/bot/workslaveholding.cpp \
    $$SRC/bot/workalchemy.cpp \
    $$SRC/bot/workcrystalgrinding.cpp


HEADERS += \
    $$SRC/tools/tools.h \
    $$SRC/tools/producer.h \
    $$SRC/tools/config.h \
    $$SRC/tools/tunedpage.h \
    $$SRC/tools/persistentcookiejar.h \
    $$SRC/tools/logger.h \
    $$SRC/tools/timebomb.h \
    $$SRC/tools/netmanager.h \
    $$SRC/tools/build_id.h \
    $$SRC/tools/activityhours.h \
    $$SRC/tools/tarball.h \
    $$SRC/tools/varmap.h \
    $$SRC/tools/treemap.h \
    $$SRC/tools/vartools.h

HEADERS  += \
    $$SRC/parsers/types.h \
    $$SRC/parsers/page_generic.h \
    $$SRC/parsers/page_login.h \
    $$SRC/parsers/page_game.h \
    $$SRC/parsers/page_game_index.h \
    $$SRC/parsers/page_game_farm.h \
    $$SRC/parsers/page_game_mine_open.h \
    $$SRC/parsers/all_pages.h \
    $$SRC/parsers/parser.h \
    $$SRC/parsers/page_game_dozor_entrance.h \
    $$SRC/parsers/page_game_dozor_onduty.h \
    $$SRC/parsers/page_game_dozor_gotvictim.h \
    $$SRC/parsers/page_game_dozor_lowhealth.h \
    $$SRC/parsers/page_game_pier.h \
    $$SRC/parsers/page_error.h \
    $$SRC/parsers/page_game_mine_main.h \
    $$SRC/parsers/page_game_mine_livefield.h \
    $$SRC/parsers/page_game_luckysquare.h \
    $$SRC/parsers/page_game_clan_treasury.h \
    $$SRC/parsers/page_game_incubator.h \
    $$SRC/parsers/page_game_fight_log.h \
    $$SRC/parsers/page_underconstruction.h \
    $$SRC/parsers/page_game_levelup.h \
    $$SRC/parsers/page_game_post_messages.h \
    $$SRC/parsers/page_game_harbor_market.h \
    $$SRC/parsers/page_game_training.h \
    $$SRC/parsers/page_game_house_plantation.h \
    $$SRC/parsers/page_game_alchemy_entrance.h \
    $$SRC/parsers/page_game_alchemy_lab.h \
    $$SRC/parsers/page_game_grinder.h \


HEADERS  += \
    $$SRC/bot/mainwindow.h \
    $$SRC/bot/alertdialog.h \
    $$SRC/bot/bot.h \
    $$SRC/bot/webactor.h \
    $$SRC/bot/farmersgroupsprices.h \
    $$SRC/bot/botstate.h \
    $$SRC/bot/work.h \
    $$SRC/bot/workwatching.h \
    $$SRC/bot/worksleeping.h \
    $$SRC/bot/workmining.h \
    $$SRC/bot/workfishing.h \
    $$SRC/bot/workfieldsopening.h \
    $$SRC/bot/workclangiving.h \
    $$SRC/bot/workfarming.h \
    $$SRC/bot/workscaryfighting.h \
    $$SRC/bot/workflyingbreeding.h \
    $$SRC/bot/worktraining.h \
    $$SRC/bot/workslaveholding.h \
    $$SRC/bot/workalchemy.h \
    $$SRC/bot/workcrystalgrinding.h

RESOURCES += \
    $$SRC/bot/bot.qrc

win32:RC_FILE += $$SRC/bot/bot_win32.rc

FORMS += \
    $$SRC/bot/alertdialog.ui

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$OBJECTS_DIR/moc
DESTDIR = $$TOP/bin







































