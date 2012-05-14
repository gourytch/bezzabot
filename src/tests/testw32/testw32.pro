CUR = $$PWD
TOP = $$CUR/../../..

CONFIG += debug
QT += core gui network webkit

TARGET=testw32
SOURCES += testw32.cpp

OBJECTS_DIR = $$TOP/obj/$$TARGET
MOC_DIR = $$TOP/obj/$$TARGET/moc
DESTDIR = $$TOP/bin

win32 {
  CONFIG += static console
  LIBS += -lwebcore -lQtCore -lversion -lshlwapi
#  -lQtWebKit -lQtGui -lQtNetwork -lQtCore -lpthread
}
