#!/bin/bash

SSH="ssh root@localhost -p 20 --"

case "$1" in
	remote)
	export LC_ALL=C
	export PATH=/opt/qt52/bin:$PATH
	cd /root/work/glimpse_client/linux
	dpkg-buildpackage -b
	;;

	*)
	# Delete files on build host
	$SSH rm -fr /root/work/*

	# Copy files via scp
	scp -q -P 20 -r . root@localhost:/root/work/

	$SSH /root/work/glimpse_client/buildscripts/build-raspberry.sh remote

	scp -q -P 20 -r root@localhost:/root/work/*.deb .
;;
esac


