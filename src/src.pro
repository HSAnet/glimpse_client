TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libclient \
           console \
           desktop \
           mobile

console.depends = libclient
desktop.depends = libclient
mobile.depends = libclient
