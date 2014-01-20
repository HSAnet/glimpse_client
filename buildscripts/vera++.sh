#!/bin/sh

cd `dirname $0`
cd ..

PROFILE_NAME="client"
PARAMS="-P max-line-length=200"

find src/ -name "*.h" -or -name "*.c" -or -name "*.cpp" | vera++ --root buildscripts/vera++ --profile $PROFILE_NAME $PARAMS $@
