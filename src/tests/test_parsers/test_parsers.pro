TARGET = test_parsers
TEMPLATE = app
CONFIG += qt debug warn_on
QT += core gui network webkit

include($${TOP_PROJECT_DIR}/common.pri)

SOURCES += test_parsers.cpp

INCLUDEPATH += $${TOP_PROJECT_DIR}/src/libbbot
LIBS += -lbbot
PRE_TARGETDEPS += $${OUTPUT_LIB_DIR}/libbbot.a
