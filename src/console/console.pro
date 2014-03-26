TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
QT -= gui network

TARGET = mplane

include(../../dist.pri)
include(../libclient/libclient.pri)

# Add the service config
linux {
    service.files = glimpse-probe.conf
    service.path = /etc/init
    INSTALLS += service
}

osx:LIBS += -framework CoreFoundation -framework DiskArbitration

unix:SOURCES += consoletools_unix.cpp
win32:SOURCES += consoletools_win.cpp

HEADERS += \
    consoletools.h

SOURCES += \
    main.cpp

OTHER_FILES += \
    glimpse-probe.conf
