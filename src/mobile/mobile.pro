TEMPLATE = app

QT += gui quick qml concurrent

include(../libclient/libclient.pri)

android {
    HEADERS += androidhelper.h
    SOURCES += androidhelper.cpp
}

SOURCES += \
    main.cpp

RESOURCES += \
    resources/resource.qrc

OTHER_FILES += \
    resources/qml/main.qml \
    resources/qml/packettrain.qml
