TEMPLATE = lib
TARGET = client
CONFIG += staticlib

QT += network concurrent

linux:!android:DEFINES += HAVE_UPNP

SOURCES +=  \
    client.cpp \
    discovery.cpp \
    connectiontester.cpp \
    requests.cpp \
    testfactory.cpp \
    tests/packettrain.cpp \
    testscheduler.cpp \
    networkhelper.cpp

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
    networkhelper.h

OTHER_FILES += \
    libclient.pri
