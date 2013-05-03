TEMPLATE = lib
TARGET = client
CONFIG += staticlib

QT += network concurrent qml

linux:!android:DEFINES += HAVE_UPNP

SOURCES +=  \
    client.cpp \
    discovery.cpp \
    connectiontester.cpp \
    requests.cpp \
    testfactory.cpp \
    tests/packettrain.cpp \
    testscheduler.cpp \
    networkhelper.cpp \
    tests/natporttimes.cpp

HEADERS += \
    client.h \
    types.h \
    discovery.h \
    connectiontester.h \
    requests.h \
    testfactory.h \
    tests/packettrain.h \
    tests/test.h \
    testscheduler.h \
    tests/testinfo.h \
    networkhelper.h \
    tests/natporttimes.h

OTHER_FILES += \
    libclient.pri
