TEMPLATE = app

QT += network widgets concurrent

mac:TARGET = mPlane

include(../libclient/libclient.pri)

SOURCES += \
    main.cpp \
    mainwindow.cpp

HEADERS += \
    mainwindow.h

FORMS += \
    mainwindow.ui

RESOURCES += \
    resources/resource.qrc

