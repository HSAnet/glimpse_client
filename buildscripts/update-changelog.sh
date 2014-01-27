#!/bin/sh

cd `dirname $0`
cd ..

git-dch --git-author -S -a

