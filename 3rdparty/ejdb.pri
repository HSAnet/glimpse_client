INCLUDEPATH += $$PWD/ejdb

win32:DEFINES += _CRT_SECURE_NO_WARNINGS

win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG -ltcejdb
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG/tcejdb.lib
} else {
    LIBS += $$BUILD_DIRECTORY/3rdparty/libtcejdb.a
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/libtcejdb.a
}
