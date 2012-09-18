TOP = ../..

TARGET = bezzabot
TEMPLATE = app

QT += core gui network webkit

win32 {
  CONFIG += console
}

SOURCES += \
    main.cpp\
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

INCLUDEPATH += ../libbbot
LIBS += -L$$TOP/lib -lbbot
PRE_TARGETDEPS += $$TOP/lib/libbbot.a

#LIBS += -lduma

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

RESOURCES += \
    bezzabot.qrc

win32:RC_FILE += bezzabot_win32.rc

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += \
        -fno-builtin-malloc \
        -fno-builtin-calloc \
        -fno-builtin-realloc \
        -fno-builtin-free
    LIBS += -ltcmalloc -lprofiler
}



































