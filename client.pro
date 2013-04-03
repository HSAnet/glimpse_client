TEMPLATE = app
CONFIG += link_pkgconfig

QT += network widgets concurrent

DEFINES += HAVE_STUN

unix:!android: {
    DEFINES += HAVE_UPNP
    #PKGCONFIG += QJson
    LIBS += -lminiupnpc
}

win32 {
    #QJSONPATH = C:\\Source\\libs\\qjson
    #LIBS += -L$$QJSONPATH\\lib -lqjson
    #INCLUDEPATH += $$QJSONPATH\\include
    LIBS += -lws2_32
}

contains(DEFINES, HAVE_STUN) {
    SOURCES += stund/stun.cxx \
               stund/udp.cxx
}

SOURCES +=  \
    negotiator.cpp \
    mainwindow.cpp \
    connectiontester.cpp

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
    mainwindow.h \
    connectiontester.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources/resource.qrc

