#! /bin/bash
TS="$(date +'%Y%m%d_%H%M%S')"
FNAME="$(cd $(dirname $0) && pwd)/src/libbbot/tools/build_id.h"
if [ ! -e $FNAME ]
then
  echo "file {$FNAME} not found" 1>&2
  exit 1
fi

cat >$FNAME <<EOF
#ifndef BUILD_ID_H
#define BUILD_ID_H

#define BUILD_ID "$TS"

#endif // BUILD_ID_H
EOF

git add $FNAME
git commit -m "BUILD_ID WAS CHANGED TO $TS"
echo "build_id was changed to $TS"

