TOP = ../..

TEMPLATE = lib
TARGET = bbot

CONFIG += qt warn_on staticlib
QT += core gui network webkit

include($${TOP_PROJECT_DIR}/common.pri)

SOURCES += \
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
    workscaryfighting.cpp \
    workflyingbreeding.cpp \
    worktraining.cpp \
    workslaveholding.cpp \
    workalchemy.cpp \
    workhealing.cpp \
    workcrystalgrinding.cpp \
    alertdialog.cpp \
    workdiving.cpp \
    parsers/page_game_atlantis.cpp \
    tools/currency.cpp \
    workquestcompletist.cpp \
    parsers/page_game_school_quests.cpp

HEADERS  += \
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
    workscaryfighting.h \
    workflyingbreeding.h \
    worktraining.h \
    workslaveholding.h \
    workalchemy.h \
    workhealing.h \
    workcrystalgrinding.h \
    alertdialog.h \
    iconames.h \
    tools/sleeper.h \
    workdiving.h \
    parsers/page_game_atlantis.h \
    tools/currency.h \
    workquestcompletist.h \
    parsers/page_game_school_quests.h

FORMS += \
    alertdialog.ui

HEADERS += \
    tools/tools.h \
    tools/producer.h \
    tools/config.h \
    tools/tunedpage.h \
    tools/persistentcookiejar.h \
    tools/logger.h \
    tools/timebomb.h \
    tools/netmanager.h \
    tools/build_id.h \
    tools/activityhours.h \
    tools/tarball.h \
    tools/varmap.h \
    tools/treemap.h \
    tools/vartools.h

SOURCES += \
    tools/tools.cpp \
    tools/config.cpp \
    tools/tunedpage.cpp \
    tools/persistentcookiejar.cpp \
    tools/logger.cpp \
    tools/timebomb.cpp \
    tools/netmanager.cpp \
    tools/activityhours.cpp \
    tools/tarball.cpp \
    tools/varmap.cpp \
    tools/treemap.cpp \
    tools/vartools.cpp

SOURCES += \
    parsers/types.cpp \
    parsers/parser.cpp \
    parsers/page_generic.cpp \
    parsers/page_error.cpp \
    parsers/page_underconstruction.cpp \
    parsers/page_login.cpp \
    parsers/page_game.cpp \
    parsers/page_game_index.cpp \
    parsers/page_game_farm.cpp \
    parsers/page_game_mine_open.cpp \
    parsers/page_game_dozor_entrance.cpp \
    parsers/page_game_dozor_onduty.cpp \
    parsers/page_game_dozor_gotvictim.cpp \
    parsers/page_game_dozor_lowhealth.cpp \
    parsers/page_game_pier.cpp \
    parsers/page_game_mine_main.cpp \
    parsers/page_game_mine_livefield.cpp \
    parsers/page_game_luckysquare.cpp \
    parsers/page_game_clan_treasury.cpp \
    parsers/page_game_incubator.cpp \
    parsers/page_game_fight_log.cpp \
    parsers/page_game_levelup.cpp \
    parsers/page_game_post_messages.cpp \
    parsers/page_game_harbor_market.cpp \
    parsers/page_game_training.cpp \
    parsers/page_game_house_plantation.cpp \
    parsers/page_game_alchemy_entrance.cpp \
    parsers/page_game_alchemy_lab.cpp \
    parsers/page_game_grinder.cpp

HEADERS += \
    parsers/types.h \
    parsers/parser.h \
    parsers/all_pages.h \
    parsers/page_generic.h \
    parsers/page_error.h \
    parsers/page_underconstruction.h \
    parsers/page_login.h \
    parsers/page_game.h \
    parsers/page_game_index.h \
    parsers/page_game_farm.h \
    parsers/page_game_mine_open.h \
    parsers/page_game_dozor_entrance.h \
    parsers/page_game_dozor_onduty.h \
    parsers/page_game_dozor_gotvictim.h \
    parsers/page_game_dozor_lowhealth.h \
    parsers/page_game_pier.h \
    parsers/page_game_mine_main.h \
    parsers/page_game_mine_livefield.h \
    parsers/page_game_luckysquare.h \
    parsers/page_game_clan_treasury.h \
    parsers/page_game_incubator.h \
    parsers/page_game_fight_log.h \
    parsers/page_game_levelup.h \
    parsers/page_game_post_messages.h \
    parsers/page_game_harbor_market.h \
    parsers/page_game_training.h \
    parsers/page_game_house_plantation.h \
    parsers/page_game_alchemy_entrance.h \
    parsers/page_game_alchemy_lab.h \
    parsers/page_game_grinder.h

#CONFIG(debug, debug|release) {
#  QMAKE_CXXFLAGS += \
#    -fno-builtin-malloc \
#    -fno-builtin-calloc \
#    -fno-builtin-realloc \
#    -fno-builtin-free
#}
