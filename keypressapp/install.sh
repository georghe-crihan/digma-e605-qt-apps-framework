#!/bin/sh
JUNKROOT=/media/mmcblk1p1
rm -f ${JUNKROOT}/junk/log.txt
exec >> ${JUNKROOT}/junk/log.txt 2>&1
uname -a
cp ${JUNKROOT}/junk/entry.order /usr/local/share/applications/more/
cp ${JUNKROOT}/junk/test.desktop /usr/local/share/applications/more/
sync
