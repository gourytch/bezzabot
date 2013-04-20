TOP_OBJ_DIR = $${TOP_BUILD_DIR}/obj

#TOP_OUTPUT_DIR = $${TOP_BUILD_DIR}/out
TOP_OUTPUT_DIR = $${TOP_BUILD_DIR}
OUTPUT_BIN_DIR = $${TOP_OUTPUT_DIR}/bin
OUTPUT_LIB_DIR = $${TOP_OUTPUT_DIR}/lib

BUILD_HASH=\"$$system("cd $${TOP_PROJECT_DIR}; git show --name-only --format='%h' | head -n1")\"
BUILD_TSTAMP=\"$$system("cd $${TOP_PROJECT_DIR}; git show --name-only --format='%ci' | head -n1 | awk '{print $1, $2}'")\"
BUILD_SUBJ=\"$$system("cd $${TOP_PROJECT_DIR}; git show --name-only --format='%s' | head -n1")\"

DEFINES += \
    BUILD_HASH="$${BUILD_HASH}" \
    BUILD_TSTAMP="$${BUILD_TSTAMP}" \
    BUILD_SUBJ="$${BUILD_SUBJ}"

OBJECTS_DIR = $${TOP_OBJ_DIR}/$${TARGET}
MOC_DIR     = $${OBJECTS_DIR}/moc

contains(TEMPLATE, lib) {
    DESTDIR = $${OUTPUT_LIB_DIR}
    message("[+] configure as library")
} else {
    message("[+] configure as application")
    DESTDIR = $${OUTPUT_BIN_DIR}
}

INCLUDEPATH += $${TOP_PROJECT_DIR}/src
LIBS += -L$${OUTPUT_LIB_DIR}

CONFIG += exceptions

#QMAKE_CXXFLAGS += \
#    -DBUILD_HASH="$${BUILD_HASH}" \
#    -DBUILD_TIME="$${BUILD_TSTAMP}" \
#    -DBUILD_SUBJ="$${BUILD_SUBJ}"
