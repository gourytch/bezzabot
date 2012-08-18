#! /bin/bash

BUILD_LX64="Y"
BUILD_LX32="N"
BUILD_WIN32="Y"
#DEPLOY_TO="$HOME/Dropbox/bezzabot-shared/"
DEPLOY_TO="$HOME/temp/bezzabot-deploy/"

MAINDIR="$(cd $(dirname $0) && pwd)"

LX32_ROOT="/srv/chroots/squeeze32"
LX32_BDIR="/root/bezzabot"

EXE_LX64="$MAINDIR/bin/bezzabot"
EXE_LX32="$LX32_BDIR/bin/bezzabot"
EXE_WIN32="$MAINDIR/../w32bezzabot/C/bezzabot/bin/solid_bot.exe"


case "$*" in
newid)
  echo "create new build id"
  cd $MAINDIR && 
  git pull && 
  ./UPDATE_bldid.sh &&
  git push 
  ;;
oldid)
  echo "keep old build id"
  ;;
*)
  echo "use $0 oldid|newid"
  exit 1
  ;;
esac

BUILD_ID=$(gawk '/#define BUILD_ID/{gsub("\"", "", $3); print $3; }' src/tools/build_id.h | xargs )
DST="$DEPLOY_TO/bid-$BUILD_ID"

echo "[*] build id: $BUILD_ID"
if [ "$BUILD_LX64" = "Y" ]
then
  echo "[*] build on host system..."
  if ! ./REBUILD.sh 
  then
    echo "[!] build failed"
    exit 1
  fi
else
  echo "[*] linux64 skipped"
fi


if [ "$BUILD_LX32" = "Y" ]
then
  echo "[*] build linux32..."
  sudo linux32 chroot /srv/chroots/squeeze32 /bin/bash --login -c "cd ~/bezzabot && git pull && ./REBUILD.sh"
  if [ ! -e /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot ]
  then
    echo "[!] build failed"
    exit 1
  fi
else
  echo  "[*] linux32 skipped"
fi

if [ "$BUILD_WIN32" = "Y" ]
then
  echo "[*] build win32..."
  ./BUILD-bb-win32-zeppelin.sh
  if [ ! -e $EXE_WIN32 ]
  then
    echo "[*] build win32 failed."
    exit 1
  fi
  reset
else
  echo "[*] win32 skipped"
fi

echo "deploy executables to $DST"

mkdir -p $DST

test -e $EXE_LX64 && cp $EXE_LX64 $DST/bezzabot-amd64
test -e $EXE_LX32 && cp $EXE_LX32 $DST/bezzabot-i686
test -e $EXE_WIN32 && cp $EXE_WIN32 $DST/bezzabot.exe

echo "[=] BEZZABOT BUILD_ID=$BUILD_ID WAS BUILT AND DEPLOYED TO $DST"
