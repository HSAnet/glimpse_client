android: {
    DEFINES += OPERATING_SYS_ANDROID
}

mac: INCLUDEPATH += $$PWD/netsnmp/mac-config
linux: INCLUDEPATH += $$PWD/netsnmp/debian-config
win32: INCLUDEPATH += $$PWD/netsnmp/win-config

INCLUDEPATH += $$PWD/netsnmp/include
DEPENDPATH += $$PWD/netsnmp/include

win32 {
    # Windows
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/3rdparty/netsnmp/$$BUILDCONFIG -lnetsnmp
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/netsnmp/$$BUILDCONFIG/netsnmp.lib
} else {
    # Other systems
    LIBS += -L$$BUILD_DIRECTORY/3rdparty/netsnmp -lnetsnmp
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/netsnmp/libnetsnmp.a
}
