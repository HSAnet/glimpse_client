TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET = mplane

include(../libclient/libclient.pri)

unix:SOURCES += consoletools_unix.cpp
win32:SOURCES += consoletools_win.cpp

HEADERS += \
    consoletools.h

SOURCES += \
    main.cpp

