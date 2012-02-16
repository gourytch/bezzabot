DEL /F /S /Q LIB OBJ BIN\*.EXE 

REM DEL /F /S /Q src\tools\build_id.h
REM git checkout src/tools/build_id.h

PATH C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin;C:\QtSDK\mingw\bin;%PATH%

qmake -recursive
make -j2 all
