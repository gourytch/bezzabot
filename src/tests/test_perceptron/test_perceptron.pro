TARGET = test_perceptron
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
CONFIG += qt debug warn_on
QT += core gui
QT -= network webkit

include($${TOP_PROJECT_DIR}/common.pri)

SOURCES += \
    test_perceptron.cpp

INCLUDEPATH += $${TOP_PROJECT_DIR}/src/libbbot
LIBS += -lbbot
PRE_TARGETDEPS += $${OUTPUT_LIB_DIR}/libbbot.a

