TEMPLATE = app
CONFIG += link_pkgconfig

QT += network

PKGCONFIG += QJson
LIBS += -lminiupnpc

SOURCES += stund/stun.cxx \
           stund/udp.cxx \
    negotiator.cpp \
    mainwindow.cpp

SOURCES += \
    main.cpp \
    client.cpp \
    server.cpp \
    floodtest.cpp \
    discovery.cpp

HEADERS += \
    client.h \
    server.h \
    testfactory.h \
    types.h \
    floodtest.h \
    discovery.h \
    negotiator.h \
    mainwindow.h

FORMS += \
    mainwindow.ui

