QT += network

INCLUDEPATH += $$PWD

android|mac:DEFINES += LIBCLIENT_STATIC

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

# If we link libclient statically, one also needs to link
# miniupnpc statically
android|mac {
    include($$SOURCE_DIRECTORY/3rdparty/qtsystems.pri)
    include($$SOURCE_DIRECTORY/3rdparty/breakpad.pri)
    include($$SOURCE_DIRECTORY/3rdparty/miniupnpc.pri)
}
