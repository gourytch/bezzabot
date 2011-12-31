TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt debug warn_on staticlib
QT     += core network webkit

TOP = ../..

HEADERS += \
    tools.h \
    producer.h \
    config.h \
    tunedpage.h \
    persistentcookiejar.h

SOURCES += \
    tools.cpp \
    config.cpp \
    tunedpage.cpp \
    persistentcookiejar.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
DESTDIR = $$TOP/lib


