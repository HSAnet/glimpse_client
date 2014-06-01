#!/bin/bash

CONTAINER_NAME=$1
BUILD_NUMBER=$2
BRANCH=$3
COMMAND=$4


function default
{
  rm -rf ./glimpse_client-$BRANCH
  mkdir glimpse_client-$BRANCH
  cp buildscripts/fedora.sh /home/buildpkg/ || true
  cp -R * glimpse_client-$BRANCH || true
  zip --update --quiet -r /home/buildpkg/glimpse_client-$BRANCH glimpse_client-$BRANCH
  sudo lxc-attach -n $CONTAINER_NAME --clear-env -- /bin/unzip -q -u -o /source/glimpse_client-$BRANCH.zip -d /source 
  sudo lxc-attach -n $CONTAINER_NAME --clear-env -- /bin/bash /source/glimpse_client-$BRANCH/buildscripts/fedora.sh $CONTAINER_NAME $BUILD_NUMBER $BRANCH remote
  cp /home/buildpkg/*.rpm .
  for file in *.rpm; do curl -v --form "channel=$CONTAINER_NAME-amd64" -F "package=@$file" "http://distributor.measure-it.net/packages/upload/" ; done
}

function remote
{
  cd `dirname $0`
  cp /source/glimpse_client-$BRANCH.zip /home/buildpkg/rpmbuild/SOURCES/
  su - buildpkg -c 'cd rpmbuild/SPECS/ && rpmbuild -ba --define "'"_branch $BRANCH"'" glimpse.spec $BUILD_NUMBER && exit'
  cp /home/buildpkg/rpmbuild/RPMS/x86_64/*.rpm /source/
  exit
}

case $COMMAND in
    remote )	remote
                ;;
    * ) default
                ;;
esac
