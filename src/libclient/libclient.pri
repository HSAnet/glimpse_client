QT += network

INCLUDEPATH += $$PWD

android|osx:DEFINES += LIBCLIENT_STATIC

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/src/libclient/$$BUILDCONFIG -lclient
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/$$BUILDCONFIG/client.lib

    # For upnp
    LIBS += -lIphlpapi
} else: mac {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += $$BUILD_DIRECTORY/src/libclient/libclient.a
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libclient.a
    } else {
        LIBS += -L$$BUILD_DIRECTORY/src/libclient -lclient
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libclient.dylib
    }
} else {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += $$BUILD_DIRECTORY/src/libclient/libclient.a
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libclient.a
    } else {
        LIBS += -L$$BUILD_DIRECTORY/src/libclient -lclient
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libclient.so
    }
}

win32:LIBS += -lws2_32
