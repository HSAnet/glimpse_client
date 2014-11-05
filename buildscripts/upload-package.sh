#!/bin/bash

PACKAGESERVER="https://distributor.measure-it.net/api/upload/"

if [ $# -ne 2 ]; then
	echo "Usage: $0 <branch> <distribution>"
	exit 1
fi

BRANCH=$1
DISTRO=$2

if [ "$BRANCH" != "develop" ]; then
	echo "Nothing to upload for branch $BRANCH"
	exit 0
fi

cd `dirname $0`
cd ..
cd ..

# Upload each file to the distributor server
for file in *.deb *.rpm *.tar.xz *.apk *.exe *.dmg; do
	if [ -e $file ]; then
		curl --form package=@$file --form channel=$DISTRO $PACKAGESERVER
	fi
done

