win32 {
    QT += systeminfo
} else: android {
} else: ios {
} else {
    INCLUDEPATH += $$PWD/qtsystems_fixes \
                   $$PWD/qtsystems_fixes/QtSystemInfo

    # Linking
    win32 {
        CONFIG(release, debug|release):BUILDCONFIG = "release"
        else:BUILDCONFIG = "debug"

        LIBS += -L$$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG -lqtsystems
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG/qtsystems.lib
    } else {
        LIBS += $$BUILD_DIRECTORY/3rdparty/libqtsystems.a
        PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/libqtsystems.a
    }
}
