TARGET = surgery
TEMPLATE = app
CONFIG += debug
QT += core gui network webkit

include($${TOP_PROJECT_DIR}/common.pri)

SOURCES += \
    appwindow.cpp \
    surgery.cpp \
    alertdialog.cpp

HEADERS += \
    appwindow.h \
    alertdialog.h

RESOURCES += \
    surgery.qrc

FORMS += \
    alertdialog.ui


INCLUDEPATH += $${TOP_PROJECT_DIR}/src/libbbot
LIBS += -lbbot
PRE_TARGETDEPS += $${OUTPUT_LIB_DIR}/libbbot.a
