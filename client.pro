TEMPLATE = subdirs

SUBDIRS += .qmake.cache.pro \
           3rdparty \
           src

OTHER_FILES += dist.pri

3rdparty.depends = .qmake.cache.pro
src.depends = 3rdparty

# Make sure "git submodule init" and "git submodule update" have been executed
!exists($$PWD/3rdparty/miniupnp/README) {
        message("Submodules missing, checking them out")
        system("git submodule update --init")
}

cache()
