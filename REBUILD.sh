#! /bin/bash

make clean;
rm -f bin/bezzabot bin/surgery bin/test_disp bin/test_parsers
rm -rf obj/ lib/
qmake -recursive &&
make clean all

