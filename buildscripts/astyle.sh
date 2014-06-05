#!/bin/bash

min_version=2.05

verlte() {
    [[  "$1" == "$(echo -e "$1\n$2" | sort -V | head -n1)" ]]
}

verlt() {
    [[ "$1" == "$2" ]] && return 1 || verlte $1 $2
}

if verlt "$(astyle --version | awk '{print $4}')" "${min_version}"; then
    echo "astyle version >=${min_version} required" >&2
    exit 1
fi

cd "$(dirname $0)"/../src
astyle -n -q -r --options=../buildscripts/astylerc *.cpp *.h
find . -name "*.cpp" -o -name "*.h" | xargs ../buildscripts/astyle.py
