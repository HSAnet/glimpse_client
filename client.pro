TEMPLATE = app
CONFIG += link_pkgconfig

# Define our master server. This can be changed for testing
MASTER_SERVER = "mplane.informatik.hs-augsburg.de"

# On osx our application is named like the bundle (in the menu)
mac:TARGET = mPlane

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += qt5

QT += network
qt5:QT += widgets concurrent

DEFINES += MASTER_SERVER=\"$$MASTERSERVER\"

unix:!android:!mac: {
    DEFINES += HAVE_UPNP
    !qt5:PKGCONFIG += QJson
    LIBS += -lminiupnpc
}

win32 {
    !qt5 {
        QJSONPATH = C:\\Source\\libs\\qjson
        LIBS += -L$$QJSONPATH\\lib -lqjson
        INCLUDEPATH += $$QJSONPATH\\include
    }
    LIBS += -lws2_32
}

SOURCES +=  \
    mainwindow.cpp \
    connectiontester.cpp \
    requests.cpp

SOURCES += \
    main.cpp \
    client.cpp \
    floodtest.cpp \
    discovery.cpp

HEADERS += \
    client.h \
    testfactory.h \
    types.h \
    floodtest.h \
    discovery.h \
    mainwindow.h \
    connectiontester.h \
    requests.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources/resource.qrc
