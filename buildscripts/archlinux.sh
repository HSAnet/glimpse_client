#!/bin/bash

CONTAINER_NAME=$1
BUILD_NUMBER=$2
BRANCH=$3
COMMAND=$4


function default
{
  rm -rf ./glimpse_client-$BRANCH
  mkdir glimpse_client-$BRANCH
  cp buildscripts/archlinux.sh /home/buildpkg/ || true
  cp -R * glimpse_client-$BRANCH || true
  sudo lxc-attach -n $CONTAINER_NAME --clear-env -- /bin/bash /source/glimpse_client-$BRANCH/buildscripts/archlinux.sh $CONTAINER_NAME $BUILD_NUMBER $BRANCH remote
  cp /home/buildpkg/*.tar.xz .
}

function remote
{
  cd `dirname $0`
  makepkg
  exit
}

case $COMMAND in
    remote )	remote
                ;;
    * ) default
                ;;
esac
