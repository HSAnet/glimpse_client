#/bin/sh

cd `dirname $0`
cd ..

BUILD_INSTALL="build-install"
REPORT_NAME="$BUILD_INSTALL/checkstyle.xml"

mkdir -p $BUILD_INSTALL

find src/ -name "*.h" -or -name "*.c" -or -name "*.cpp" | vera++ --checkstyle-report $REPORT_NAME
