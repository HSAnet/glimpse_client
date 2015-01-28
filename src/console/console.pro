TEMPLATE = app
TARGET = glimpse-console
CONFIG += console
CONFIG -= app_bundle
QT -= gui network

TARGET = mplane

include($$SOURCE_DIRECTORY/dist.pri)
include($$SOURCE_DIRECTORY/src/libclient/libclient.pri)

# Add the service config
linux {
    service.files = glimpse-probe.conf
    service.path = /etc/init
    INSTALLS += service
}

osx {
    LIBS += -framework SystemConfiguration \
            -framework Foundation \
            -framework AppKit \
            -framework IOKit  \
            -framework QTKit \
            -framework CoreWLAN \
            -framework CoreLocation \
            -framework CoreFoundation \
            -framework ScreenSaver \
            -framework IOBluetooth \
            -framework CoreServices \
            -framework DiskArbitration \
            -framework ApplicationServices
}

unix:SOURCES += consoletools_unix.cpp
win32:SOURCES += consoletools_win.cpp

HEADERS += \
    consoletools.h

SOURCES += \
    main.cpp

OTHER_FILES += \
    glimpse-probe.conf
