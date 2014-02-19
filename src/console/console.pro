TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET = mplane

include(../../dist.pri)
include(../libclient/libclient.pri)

osx:LIBS += -framework CoreFoundation -framework DiskArbitration

unix:SOURCES += consoletools_unix.cpp
win32:SOURCES += consoletools_win.cpp

HEADERS += \
    consoletools.h

SOURCES += \
    main.cpp

