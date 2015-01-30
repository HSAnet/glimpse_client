#!/bin/bash

cd `dirname $0`
cd ..

function getVersion {
	local major=$(sed -nE 's/versionMajor = ([0-9]+);/\1/p' src/libclient/client.h | awk '{print $4}')
	local minor=$(sed -nE 's/versionMinor = ([0-9]+);/\1/p' src/libclient/client.h | awk '{print $4}')
	local patch=$(sed -nE 's/versionPatch = ([0-9]+);/\1/p' src/libclient/client.h | awk '{print $4}')
	
	echo "$major.$minor.$patch"
}

MAINTAINER="Christoph Keller <gri@not-censored.com>"
VERSION=$(getVersion)
TIMESTAMP=$(date -R)

BUILDNUMBER=$1


# Debian/Ubuntu/Mint
cd debian

if [ "$BUILDNUMBER" = "" ]; then
	echo "You have no BUILDNUMBER variable set, exiting"
	exit 1
fi

echo "glimpse ($VERSION-$BUILDNUMBER) unstable; urgency=low" > changelog
echo "" >> changelog
echo "  * Snapshot build #$BUILDNUMBER" >> changelog
echo "" >> changelog
echo " -- $MAINTAINER  $TIMESTAMP" >> changelog

echo "Changelog for build #$BUILDNUMBER written."


# Fedora/CentOS/OpenSuse
cd ..
sed "s/^Version:.*/Version:		$VERSION/" -i glimpse_client.spec
sed "s/^Release:.*/Release:		$BUILDNUMBER/" -i glimpse_client.spec


# Archlinux
sed "s/^pkgver=.*/pkgver=$VERSION.$BUILDNUMBER/" -i PKGBUILD


# Windows
sed -E "s/(#define MyAppVersion) \"(.*)\"/\1 \"$VERSION-$BUILDNUMBER\"/" -i setup/glimpse.iss
