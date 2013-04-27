TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += libclient \
           console \
           desktop

android:SUBDIRS += mobile
