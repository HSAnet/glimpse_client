TEMPLATE = subdirs

SUBDIRS += 3rdparty \
           src

src.depends = 3rdparty

# Make sure "git submodule init" and "git submodule update" have been executed
!exists($$PWD/3rdparty/miniupnp/README) {
        message("Submodules missing, checking them out")
        system("git submodule init")
        system("git submodule update")
}

cache()
