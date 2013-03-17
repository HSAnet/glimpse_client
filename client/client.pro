TEMPLATE = app
CONFIG += link_pkgconfig

QT += network

PKGCONFIG += QJson
LIBS += -lminiupnpc

SOURCES += stund/stun.cxx \
           stund/udp.cxx

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
    discovery.h

