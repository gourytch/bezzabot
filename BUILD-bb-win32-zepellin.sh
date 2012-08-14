#! /bin/bash
source ../w32bezzabot/rc

cd ../w32bezzabot/C/bezzabot && 
git pull && 
wine cmd /c REBUILD-win32-zeppelin.cmd
