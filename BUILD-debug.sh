#! /bin/bash

cd $(dirname $0)
test -d build-debug && rm -rf build-debug
mkdir -p build-debug
cd build-debug
qmake ../bezzabot.pro CONFIG+=debug CONFIG-=release
make -j5 all
