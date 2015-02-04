#!/bin/bash

PACKAGESERVER="https://distributor.measure-it.net/api/upload/"

if [ $# -ne 2 ]; then
	echo "Usage: $0 <branch> <distribution>"
	exit 1
fi

BRANCH=$1
DISTRO=$2

cd `dirname $0`
cd ..

# Check whether the distributor wants this upload or not
HTTP_CODE=$(curl -s -o /dev/null -w \"%{http_code}\" $PACKAGESERVER?branch=$BRANCH&channel=$DISTRO)
if [ "$HTTP_CODE" != "\"200\"" ]; then
	echo "Branch $BRANCH not wanted by the server: $HTTP_CODE"
	exit 0
fi

if [ "$OS" == "Windows_NT" ]; then
	cd setup
	cd Output
	
	for file in *.exe; do
		echo "Uploading $file"
		
		curl --form package=@$file --form channel=$DISTRO --form branch=$BRANCH $PACKAGESERVER
	done
else
	cd ..
	cd build || true

	# Upload each file to the distributor server
	for file in *.deb *.rpm *.tar.xz *.apk *.exe *.dmg; do
		if [ -e $file ]; then
			echo "Uploading $file"
			
			curl --form package=@$file --form channel=$DISTRO --form branch=$BRANCH $PACKAGESERVER
		fi
	done
fi
