#! /bin/bash
sudo linux32 chroot /srv/chroots/squeeze32 /bin/bash --login -c "cd ~/bezzabot && git pull && ./REBUILD.sh"
if [ -e /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot ]
then
  rm -f /mnt/auto/sshfs/beee-gour/home/gour/bezzabot/bezzabot-i686_
  mv /mnt/auto/sshfs/beee-gour/home/gour/bezzabot/bezzabot-i686{,_}
  cp /srv/chroots/squeeze32/root/bezzabot/bin/bezzabot /mnt/auto/sshfs/beee-gour/home/gour/bezzabot/bezzabot-i686
  echo "OK"
else
  echo "FAIL"
fi

