QT += network

INCLUDEPATH += $$PWD

android|mac:DEFINES += LIBCLIENT_STATIC

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/src/libclient/$$BUILDCONFIG -lglimpse
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/$$BUILDCONFIG/glimpse.lib

    # For upnp
    LIBS += -lIphlpapi
} else: mac {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += $$BUILD_DIRECTORY/src/libclient/libglimpse.a
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libglimpse.a
    } else {
        LIBS += -L$$BUILD_DIRECTORY/src/libclient -lglimpse
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libglimpse.dylib
    }
} else {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += $$BUILD_DIRECTORY/src/libclient/libglimpse.a
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libcglimpse.a
    } else {
        LIBS += -L$$BUILD_DIRECTORY/src/libclient -lglimpse
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/src/libclient/libglimpse.so
    }
}

win32:LIBS += -lws2_32

# If we link libclient statically, one also needs to link
# miniupnpc statically
android|mac {
    include($$SOURCE_DIRECTORY/3rdparty/qtsystems.pri)
    include($$SOURCE_DIRECTORY/3rdparty/breakpad.pri)
    include($$SOURCE_DIRECTORY/3rdparty/miniupnpc.pri)
    include($$SOURCE_DIRECTORY/3rdparty/netsnmp.pri)
}
