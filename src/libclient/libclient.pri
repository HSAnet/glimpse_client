QT += network

INCLUDEPATH += $$PWD

android|mac:DEFINES += LIBCLIENT_STATIC

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L../libclient/$$BUILDCONFIG -lclient
    PRE_TARGETDEPS = ../libclient/$$BUILDCONFIG/client.lib

    # For upnp
    LIBS += -lIphlpapi
} else: mac {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += ../libclient/libclient.a
        PRE_TARGETDEPS = ../libclient/libclient.a
    } else {
        LIBS += -L../libclient -lclient
        PRE_TARGETDEPS = ../libclient/libclient.dylib
    }
} else {
    contains(DEFINES, LIBCLIENT_STATIC) {
        LIBS += ../libclient/libclient.a
        PRE_TARGETDEPS = ../libclient/libclient.a
    } else {
        LIBS += -L../libclient -lclient
        PRE_TARGETDEPS = ../libclient/libclient.so
    }
}

win32:LIBS += -lws2_32

# If we link libclient statically, one also needs to link
# miniupnpc statically
android|mac:include(../../3rdparty/miniupnpc.pri)
