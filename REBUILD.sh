#! /bin/bash

make distclean;
rm -f bin/bezzabot bin/surgery bin/test_disp bin/test_parsers
rm -rf obj/ lib/
qmake -recursive &&
make clean && 
make -j2 all 

