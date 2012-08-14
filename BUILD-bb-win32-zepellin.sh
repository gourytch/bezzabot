#! /bin/bash
source ../w32bezzabot/rc

cd ../w32bezzabot/C/bezzabot && 
git pull && 
wine cmd /c BUILD-bb-win32-zeppelin.cmd
