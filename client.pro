TEMPLATE = subdirs

SUBDIRS += 3rdparty \
           src

src.depends = 3rdparty
