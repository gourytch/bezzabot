#! /bin/bash
W32BASE="$HOME/src/w32bezzabot"
source $W32BASE/rc

cd $W32BASE/C/bezzabot && 
git pull && 
wine cmd /c BUILD-bb-win32-zeppelin.cmd
echo "all done. enter \"reset\" for restore terminal' funtionality =)"
