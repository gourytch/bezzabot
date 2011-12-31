TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt debug warn_on staticlib
QT     += core webkit

TOP = ../..

HEADERS += \
    tools.h \
    types.h \
    page_generic.h \
    page_login.h \
    page_game.h \
    page_game_index.h \
    page_game_farm.h \
    page_game_mine_open.h \
    all_pages.h \
    parser.h

SOURCES += \
    tools.cpp \
    types.cpp \
    page_generic.cpp \
    page_login.cpp \
    page_game.cpp \
    page_game_index.cpp \
    page_game_farm.cpp \
    page_game_mine_open.cpp \
    parser.cpp

OBJECTS_DIR = $$TOP/obj
DESTDIR = $$TOP/lib

QMAKE_EXTRA_TARGETS += install

install.commands = \
	mkdir -p $$TOP/include/parsers/; \
	cp -f *.h $$TOP/include/parsers/;

