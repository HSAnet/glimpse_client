TEMPLATE = app

QT += gui quick qml concurrent

include(../libclient/libclient.pri)

SOURCES += \
    main.cpp

RESOURCES += \
    resources/resource.qrc

OTHER_FILES += \
    resources/qml/main.qml

