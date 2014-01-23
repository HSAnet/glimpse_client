win32 {
} else: osx {
} else: ios {
} else: android {
} else: linux {
    # Set the rpath, otherwise we can't load the client applications without changing LD_LIBRARY_PATH
    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,\$\$ORIGIN\'

    target.path = /opt/glimpse
    INSTALLS += target
} else {
    error(No distribution for this platform defined)
}
