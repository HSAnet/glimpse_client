TEMPLATE = app
CONFIG += link_pkgconfig

greaterThan(QT_MAJOR_VERSION, 4): CONFIG += qt5

QT += network
qt5:QT += widgets concurrent

DEFINES += HAVE_STUN

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

android:OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/drawable/icon.png \
    android/res/drawable/logo.png \
    android/res/drawable-hdpi/icon.png \
    android/res/drawable-ldpi/icon.png \
    android/res/drawable-mdpi/icon.png \
    android/res/layout/splash.xml \
    android/res/values/libs.xml \
    android/res/values/strings.xml \
    android/res/values-de/strings.xml \
    android/res/values-el/strings.xml \
    android/res/values-es/strings.xml \
    android/res/values-et/strings.xml \
    android/res/values-fa/strings.xml \
    android/res/values-fr/strings.xml \
    android/res/values-id/strings.xml \
    android/res/values-it/strings.xml \
    android/res/values-ja/strings.xml \
    android/res/values-ms/strings.xml \
    android/res/values-nb/strings.xml \
    android/res/values-nl/strings.xml \
    android/res/values-pl/strings.xml \
    android/res/values-pt-rBR/strings.xml \
    android/res/values-ro/strings.xml \
    android/res/values-rs/strings.xml \
    android/res/values-ru/strings.xml \
    android/res/values-zh-rCN/strings.xml \
    android/res/values-zh-rTW/strings.xml \
    android/src/org/kde/necessitas/ministro/IMinistro.aidl \
    android/src/org/kde/necessitas/ministro/IMinistroCallback.aidl \
    android/src/org/kde/necessitas/origo/QtActivity.java \
    android/src/org/kde/necessitas/origo/QtApplication.java \
    android/version.xml

