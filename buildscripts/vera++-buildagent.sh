#!/bin/sh

cd `dirname $0`
cd ..

BUILD_INSTALL="build-install"
REPORT_NAME="$BUILD_INSTALL/checkstyle.xml"

mkdir -p $BUILD_INSTALL

buildscripts/vera++.sh --checkstyle-report $REPORT_NAME

exit 0

