#! /bin/bash
BIN=/srv/chroots/squeeze32/root/bezzabot/bin/bezzabot
DST=/mnt/auto/sshfs/beee-gour/home/gour/bezzabot/bezzabot-i686
BAK=${DST}_bak_$(date +'%Y%m%d%H%M%S')

sudo linux32 chroot /srv/chroots/squeeze32 /bin/bash --login -c "cd ~/bezzabot && git pull && ./REBUILD.sh"
if [ -e $BIN ]
then
  rm -f $BAK
  mv $DST $BAK
  cp $BIN $DST
  echo "OK"
else
  echo "FAIL"
fi

