#! /bin/bash
W32BASE="$HOME/src/w32bezzabot"
source $W32BASE/rc

cd $W32BASE/C/bezzabot && 
git pull && 
wine cmd /c REBUILD-win32-zeppelin.cmd
