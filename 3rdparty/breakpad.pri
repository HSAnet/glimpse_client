BREAKPAD_PATH = $$PWD/breakpad

INCLUDEPATH += $$BREAKPAD_PATH \
               $$BREAKPAD_PATH/src

android:INCLUDEPATH += $$BREAKPAD_PATH/src/common/android/include
android:QMAKE_CXXFLAGS += -std=gnu++0x

# Breakpad does not work on ios yet (for us)
!ios {
    CONFIG += breakpad-builtin
}

breakpad-builtin {
    DEFINES += HAVE_BREAKPAD

    *clang*|*g++*|*llvm*: QMAKE_CXXFLAGS += -g
    *msvc*: QMAKE_CXXFLAGS += /Zi
}

# Linking
win32 {
    CONFIG(release, debug|release):BUILDCONFIG = "release"
    else:BUILDCONFIG = "debug"

    LIBS += -L$$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG -lbreakpad
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/$$BUILDCONFIG/breakpad.lib
} else:!ios {
    LIBS += $$BUILD_DIRECTORY/3rdparty/libbreakpad.a
    PRE_TARGETDEPS = $$BUILD_DIRECTORY/3rdparty/libbreakpad.a
}
