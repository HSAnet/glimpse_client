#!/bin/sh

cd `dirname $0`
cd ..

cppcheck src/ -q --enable=all --error-exitcode=1

