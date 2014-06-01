#/bin/sh

zip glimpse_client-develop *

cp glimpse_client-develop.zip /home/buildpkg/ 

sudo lxc-attach -n fedora

cp glimpse_client-develop.zip rpmbuild/SOURCES/

cd rpmbuild/SPECS

rpmbuild -ba glimpse.spec

exit

#don't forget to copy artifacts
