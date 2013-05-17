TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS += 3rdparty \
           src

src.depends = 3rdparty
