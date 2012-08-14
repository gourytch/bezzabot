DEL /F /S /Q BIN\BEZZABOT.EXE 

REM DEL /F /S /Q src\tools\build_id.h
REM git checkout src/tools/build_id.h

PATH C:\QtSDK\Desktop\Qt\4.8.1\mingw\bin;C:\QtSDK\mingw\bin;C:\MinGW\bin;C:\MinGW\mingw32\bin;C:\MinGW\msys\1.0\bin;C:\MinGW\msys\1.0\sbin;%PATH%

CD SRC\SOLID_BOT
qmake
make -j5 all

REM qmake -recursive
REM make -C src/tools all
REM make -C src/parsers all
REM make -C src/bot all
