TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libclient \
           mobile

!android {
    SUBDIRS += console \
               desktop
}

console.depends = libclient
desktop.depends = libclient
mobile.depends = libclient
