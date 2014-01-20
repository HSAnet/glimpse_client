#!/bin/sh

cppcheck src/ -q --enable=all --error-exitcode=1

