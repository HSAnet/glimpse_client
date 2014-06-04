#!/bin/bash

PACKAGESERVER="repository@debmirror.informatik.hs-augsburg.de"
REPOSITORYPATH="repository"

if [ $# -ne 1 ]; then
	echo "Usage: $0 <branch>"
	exit 1
fi

BRANCH=$1

if [ "$BRANCH" != "develop" ]; then
	echo "Nothing to upload for branch $BRANCH"
	exit 0
fi

cd `dirname $0`
cd ..
cd ..

# Copy files via scp
scp *_amd64.deb $PACKAGESERVER:/home/repository/$REPOSITORYPATH/binary || true
scp *_armhf.deb $PACKAGESERVER:/home/repository/$REPOSITORYPATH/raspberrypi || true

# Update the Packages.gz
ssh $PACKAGESERVER "$REPOSITORYPATH/update_packages.sh"
