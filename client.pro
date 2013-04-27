TEMPLATE = app
CONFIG += link_pkgconfig

# On osx our application is named like the bundle (in the menu)
mac:TARGET = mPlane

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += qt5

QT += network
qt5:QT += widgets concurrent

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
    main.cpp \
    client.cpp \
    discovery.cpp \
    mainwindow.cpp \
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
    mainwindow.h \
    connectiontester.h \
    requests.h \
    testfactory.h \
    tests/packettrain.h \
    tests/test.h \
    testscheduler.h \
    tests/testinfo.h \
    networkhelper.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources/resource.qrc
