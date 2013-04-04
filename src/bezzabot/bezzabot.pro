TARGET = bezzabot
TEMPLATE = app
QT += core gui network webkit

win32 {
  CONFIG += console
}

include($${TOP_PROJECT_DIR}/common.pri)

SOURCES += \
    main.cpp\
    mainwindow.cpp

HEADERS  += \
    mainwindow.h

INCLUDEPATH += $${TOP_PROJECT_DIR}/src/libbbot
LIBS += -lbbot
PRE_TARGETDEPS += $${OUTPUT_LIB_DIR}/libbbot.a

#LIBS += -lduma

RESOURCES += \
    bezzabot.qrc

win32:RC_FILE += bezzabot_win32.rc

CONFIG(debug, debug|release) {
    QMAKE_CXXFLAGS += \
        -fno-builtin-malloc \
        -fno-builtin-calloc \
        -fno-builtin-realloc \
        -fno-builtin-free
#  linux {
#    LIBS += -ltcmalloc -lprofiler
#  }
}
