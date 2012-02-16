DEL /F /S /Q LIB OBJ BIN\*.EXE 

REM DEL /F /S /Q src\tools\build_id.h
REM git checkout src/tools/build_id.h

qmake -recursive
make -j2 all
