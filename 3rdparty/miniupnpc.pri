DEFINES += HAVE_UPNP MINIUPNP_STATICLIB MINIUPNPC_SET_SOCKET_TIMEOUT

android:DEFINES += sun # Atleast needed for android
else:linux:DEFINES += _GNU_SOURCE
mac:DEFINES += _DARWIN_C_SOURCE
win32:DEFINES += _CRT_SECURE_NO_WARNINGS

#linux:LIBS += -lminiupnpc

android|mac|win32|linux {
    INCLUDEPATH += $$PWD/miniupnp
    win32:INCLUDEPATH += $$PWD/miniupnp_windows_fixes
}

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG -lminiupnp
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG/miniupnp.lib
} else {
    LIBS += $$BUILD_DIRECTORY/3rdparty/libminiupnp.a
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/libminiupnp.a
}
