DEL /F /S /Q LIB OBJ BIN\*.EXE 

REM DEL /F /S /Q src\tools\build_id.h
REM git checkout src/tools/build_id.h

PATH C:\QtSDK\Desktop\Qt\4.7.3\mingw\bin;C:\QtSDK\mingw\bin;C:\MinGW\bin;C:\MinGW\mingw32\bin;C:\MinGW\msys\1.0\bin;C:\MinGW\msys\1.0\sbin;%PATH%

qmake -recursive
make -j2 all
