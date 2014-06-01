#!/bin/bash

if [ "$#" -ne 3 ]; then
    echo "First parameter has to be a DISTRO\n"
    echo "Second parameter is the build number"
    echo "Third parameter is the build branch"
fi

DISTRO=$1
BUILD_NUMBER=$2
BRANCH=$3

echo $PWD

case $DISTRO in 
    fedora )
        bash buildscripts/fedora.sh fedora $BUILD_NUMBER $BRANCH local rpm
        ;;
    debian )
        bash buildscripts/debian.sh debian $BUILD_NUMBER $BRANCH local deb
        ;;
    ubuntu )
        bash buildscripts/debian.sh ubuntu $BUILD_NUMBER $BRANCH local deb
        ;;
    gentoo ) 
        bash buildscripts/gentoo.sh $BUILD_NUMBER "ebuild"
        ;;
    archlinux )
        bash buildscripts/archlinux.sh $BUILD_NUMBER "pkg.tar.xz"
        ;;
    opensuse )
        bash buildscripts/opensuse.sh opensuse $BUILD_NUMBER $BRANCH local rpm
        ;;
    * ) 
        echo "No distro $DISTRO configured!"
        ;;
esac
