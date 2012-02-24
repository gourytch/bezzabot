#! /bin/bash
TS="$(date +'%Y%m%d_%H%M%S')"
DST="$HOME/Dropbox/bezzabot-shared/$TS"
mkdir -p $DST

cd ~/src/toys/botva/bezzabot && 
git pull && 
./UPDATE_bldid.sh &&
./REBUILD.sh || exit
git push 
sudo linux32 chroot /srv/chroots/squeeze32 /bin/bash --login -c "cd ~/bezzabot && git pull && ./REBUILD.sh"
test -e /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot || exit 1
cd /home/gour/toys/wine-env/drive_c/bezzabot/ && git pull && wine cmd /c REBUILD-win32.cmd
test -e /home/gour/toys/wine-env/drive_c/bezzabot/bin/bezzabot.exe || exit 1
reset
cp /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot $DST/bezzabot-i686
cp /home/gour/toys/wine-env/drive_c/bezzabot/bin/bezzabot.exe $DST
echo "BEZZABOT WAS DEPLOYED TO $TS subdir"

