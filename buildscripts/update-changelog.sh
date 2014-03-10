#!/bin/sh

MAINTAINER="Christoph Keller <gri@not-censored.com>"
VERSION="0.1alpha1"
TIMESTAMP=$(date -R)

BUILDNUMBER=$1

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

