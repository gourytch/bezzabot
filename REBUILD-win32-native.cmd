DEL /F /S /Q LIB OBJ BIN\*.EXE 
DEL /F /S /Q RELEASE
MKDIR RELEASE

REM DEL /F /S /Q src\tools\build_id.h
REM git checkout src/tools/build_id.h

SET QTVER=4.8.1
PATH C:\QtSDK\Desktop\Qt\%QTVER%\mingw\bin;C:\QtSDK\mingw\bin;C:\MinGW\bin;C:\MinGW\mingw32\bin;C:\MinGW\msys\1.0\bin;C:\MinGW\msys\1.0\sbin;%PATH%

CD RELEASE
qmake -r ../bezzabot.pro CONFIG+=release CONFIG-=debug CONFIG-=declarative_debug
make release
