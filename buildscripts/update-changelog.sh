#!/bin/sh

MAINTAINER="Christoph Keller <gri@not-censored.com>"
VERSION="0.1alpha1"
TIMESTAMP=$(date -R)

BUILDNUMBER=$1


# Debian/Ubuntu/Mint
cd `dirname $0`
cd ..
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
sed "s/^pkgver=.*/pkgver=$VERSION/" -i PKGBUILD
