TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libclient \
           mobile

!android:!ios {
    SUBDIRS += console \
               desktop
}

console.depends = libclient
desktop.depends = libclient
mobile.depends = libclient
