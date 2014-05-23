#/bin/sh

LXC="/var/lib/lxc/fedora/rootfs/home/buildpkg/rpmbuild/SOURCES/"

zip glimpse_client-develop *

cp glimpse_client-develop.zip $LXC

sudo lxc-attach -n fedora

cd rpmbuild/SPECS

rpmbuild -ba glimpse.spec

exit

cp $LXC .
