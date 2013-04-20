#! /bin/bash

set -e
set -x

GWBASE="/opt/mingw32"
BUILD="$(cd $(dirname $0) && pwd)/x-mingw32-release"

test -d "$BUILD" && rm -rf "$BUILD"
mkdir -p "$BUILD"
cd "$BUILD"

export PATH="$GWBASE/bin:$PATH"
export LD_LIBRARY_PATH="$GWBASE/lib:$PATH"
export PKG_CONFIG_PATH="$GWBASE/lib/pkgconfig:$PKG_CONFIG_PATH"

$GWBASE/bin/qmake -r \
	CONFIG+=release \
	CONFIG-=debug \
	CONFIG-=declarative_debug \
	../bezzabot.pro && make release
