TEMPLATE = subdirs

SUBDIRS += 3rdparty \
           src

OTHER_FILES += dist.pri

src.depends = 3rdparty

# Make sure "git submodule init" and "git submodule update" have been executed
!exists($$PWD/3rdparty/miniupnp/README) {
        message("Submodules missing, checking them out")
        system("git submodule update --init")
}

cache()
