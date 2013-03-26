TEMPLATE = app
CONFIG += link_pkgconfig

QT += network


unix {
    DEFINES += HAVE_UPNP HAVE_STUN
    PKGCONFIG += QJson
    LIBS += -lminiupnpc

    SOURCES += stund/stun.cxx \
               stund/udp.cxx
}

win32 {
    QJSONPATH = C:\\Source\\libs\\qjson
    LIBS += -L$$QJSONPATH\\lib -lqjson
    INCLUDEPATH += $$QJSONPATH\\include
}

SOURCES +=  \
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

