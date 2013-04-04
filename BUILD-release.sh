#! /bin/bash

cd $(dirname $0)
test -d build-release && rm -rf build-release
mkdir -p build-release
cd build-release
qmake ../bezzabot.pro CONFIG-=debug CONFIG+=release
make -j5 all
