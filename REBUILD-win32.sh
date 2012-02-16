#! /bin/bash

make clean;
rm -f bin/bezzabot.exe bin/surgery.exe bin/test_disp.exe bin/test_parsers.exe
rm -rf obj/ lib/
i686-pc-mingw32-qmake -recursive &&
make clean &&
make -j2 all

