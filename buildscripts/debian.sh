#!/bin/bash

MAINTAINER="Christoph Keller <gri@not-censored.com>"
VERSION="0.1alpha1"
TIMESTAMP=$(date -R)

CONTAINER_NAME=$1
BUILDNUMBER=$2
BRANCH=$3
COMMAND=$4

function default 
{
	changelog
	
	rm -rf ./glimpse_client-$BRANCH
	mkdir glimpse_client-$BRANCH
  	cp buildscripts/debian.sh /home/buildpkg/ || true
	cp -R * glimpse_client-$BRANCH || true	

	sudo lxc-attach -n $CONTAINER_NAME --clear-env -- /bin/bash /source/glimpse_client-$BRANCH/buildscripts/debian.sh $CONTAINER_NAME $BUILD_NUMBER $BRANCH remote && upload
}

function changelog
{
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
}

function remote
{
	cd `dirname $0`
	cd ..
	dpkg-buildpackage -b
	exit
}

function upload
{
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
}

case $COMMAND in
    remote )    remote
                ;;
    * ) default
                ;;
esac
