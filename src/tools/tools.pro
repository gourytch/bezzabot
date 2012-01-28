TEMPLATE = lib
LANGUAGE = C++
CONFIG += qt debug warn_on staticlib
QT     += core network webkit


win32 {
  CONFIG -= debug
  CONFIG += console
}

TOP = ../..

HEADERS += \
    tools.h \
    producer.h \
    config.h \
    tunedpage.h \
    persistentcookiejar.h \
    logger.h \
    timebomb.h \
    netmanager.h \
    build_id.h \
    activityhours.h

SOURCES += \
    tools.cpp \
    config.cpp \
    tunedpage.cpp \
    persistentcookiejar.cpp \
    logger.cpp \
    timebomb.cpp \
    netmanager.cpp \
    activityhours.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/lib







