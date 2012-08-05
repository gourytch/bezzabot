#! /bin/bash

MAINDIR="$HOME/src/toys/botva/bezzabot"

TS="$(date +'%Y%m%d_%H%M%S')"
DST="$HOME/Dropbox/bezzabot-shared/$TS"
mkdir -p $DST

cd $MAINDIR && 
git pull && 
./UPDATE_bldid.sh &&
./REBUILD.sh || exit
git push 
sudo linux32 chroot /srv/chroots/squeeze32 /bin/bash --login -c "cd ~/bezzabot && git pull && ./REBUILD.sh"
test -e /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot || exit 1
cd /home/gour/toys/wine-env/drive_c/bezzabot/ && git pull && wine cmd /c REBUILD-win32.cmd
test -e /home/gour/toys/wine-env/drive_c/bezzabot/bin/bezzabot.exe || exit 1
reset
cp $MAINDIR/bin/bezzabot $DST/bezzabot-amd64
cp /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot $DST/bezzabot-i686
cp /home/gour/toys/wine-env/drive_c/bezzabot/bin/bezzabot.exe $DST

if false; then
BIN=$DST/bezzabot-i686
EEE=/mnt/auto/sshfs/beee-gour/home/gour/bezzabot/bezzabot-i686
else
BIN=$DST/bezzabot-amd64
EEE=/srv/toys/bezzabot/bezzabot-amd64
fi

BAK=${EEE}_bak_$TS
if [ -e $BIN ]
then
  rm -f $BAK
  mv $EEE $BAK
  cp $BIN $EEE
  echo "OK"
else
  echo "FAIL"
fi


echo "BEZZABOT WAS DEPLOYED TO $TS subdir"

