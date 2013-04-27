TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle

TARGET = mplane

include(../libclient/libclient.pri)

SOURCES += \
    main.cpp
