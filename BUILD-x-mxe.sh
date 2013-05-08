#! /bin/bash

set -e
set -x
source /opt/mxe/rc

BUILD="$(cd $(dirname $0) && pwd)/x-mxe-release"

test -d "$BUILD" && rm -rf "$BUILD"
mkdir -p "$BUILD"
cd "$BUILD"

$MXE_ARCH-qmake -r \
	CONFIG+=release \
	CONFIG-=debug \
	CONFIG-=declarative_debug \
	../bezzabot.pro && make release
