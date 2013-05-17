QT += network

INCLUDEPATH += $$PWD

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L../libclient/$$BUILDCONFIG -lclient
    PRE_TARGETDEPS = ../libclient/$$BUILDCONFIG/client.lib

    # For upnp
    LIBS += -lIphlpapi
}
else {
    LIBS += ../libclient/libclient.a
    PRE_TARGETDEPS = ../libclient/libclient.a
}

linux:!android:LIBS += -lminiupnpc
win32:LIBS += -lws2_32
