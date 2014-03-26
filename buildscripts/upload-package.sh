#!/bin/bash

PACKAGESERVER="repository@debmirror.informatik.hs-augsburg.de"
REPOSITORYPATH="repository"

cd `dirname $0`
cd ..
cd ..

# Copy files via scp
scp *_amd64.deb $PACKAGESERVER:/home/repository/$REPOSITORYPATH/binary || true
scp *_armhf.deb $PACKAGESERVER:/home/repository/$REPOSITORYPATH/raspberrypi || true

# Update the Packages.gz
ssh $PACKAGESERVER "$REPOSITORYPATH/update_packages.sh"
