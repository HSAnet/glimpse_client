TEMPLATE = lib
TARGET = client
CONFIG += staticlib

QT += network concurrent qml quick

staticlib:DEFINES += LIBCLIENT_STATIC
else:DEFINES += LIBCLIENT_BUILD

INCLUDEPATH += $$PWD

include(../../3rdparty/miniupnpc.pri)
include(../../3rdparty/breakpad.pri)

android {
    QT += androidextras

    HEADERS += androidhelper.h \
               androidimageprovider.h \
               androidprocessmodel.h
    SOURCES += androidhelper.cpp \
               storage/storagepaths_android.cpp \
               log/logger_android.cpp \
               deviceinfo_android.cpp \
               androidimageprovider.cpp \
               androidprocessmodel.cpp
} else: ios {
    SOURCES += log/logger_all.cpp
    OBJECTIVE_SOURCES += storage/storagepaths_ios.mm \
                         deviceinfo_ios.mm
} else {
    SOURCES += storage/storagepaths.cpp \
               log/logger_all.cpp

    unix: SOURCES += deviceinfo_unix.cpp
    else: SOURCES += deviceinfo.cpp
}

osx {
    QT += macextras
    LIBS += -framework AppKit

    HEADERS += macprocessmodel.h \
               macimageprovider.h
    OBJECTIVE_SOURCES += macprocessmodel.mm \
                         macimageprovider.mm
}

breakpad-builtin {
    HEADERS += crashhandler.h
    ios:OBJECTIVE_SOURCES += crashhandler_ios.mm
    else:SOURCES += crashhandler.cpp
}

SOURCES +=  \
    client.cpp \
    connectiontester.cpp \
    tests/packettrain.cpp \
    networkhelper.cpp \
    tests/natporttimes.cpp \
    settings.cpp \
    webrequester.cpp \
    tests/speedtest.cpp \
    response.cpp \
    network/requests/request.cpp \
    network/requests/userregisterrequest.cpp \
    network/requests/registerdevicerequest.cpp \
    network/requests/manualrequest.cpp \
    network/requests/loginrequest.cpp \
    network/requests/getconfigrequest.cpp \
    timing/timingfactory.cpp \
    timing/timing.cpp \
    timing/periodictiming.cpp \
    timing/onofftiming.cpp \
    timing/immediatetiming.cpp \
    timing/calendartiming.cpp \
    scheduler/schedulerstorage.cpp \
    scheduler/schedulermodel.cpp \
    scheduler/scheduler.cpp \
    report/reportstorage.cpp \
    report/reportscheduler.cpp \
    report/report.cpp \
    task/taskvalidator.cpp \
    task/taskexecutor.cpp \
    task/task.cpp \
    task/result.cpp \
    network/networkmanager.cpp \
    measurement/measurementfactory.cpp \
    measurement/measurement.cpp \
    measurement/measurementdefinition.cpp \
    measurement/btc/btc_mp.cpp \
    measurement/btc/btc_ma.cpp \
    measurement/btc/btc_definition.cpp \
    network/udpsocket.cpp \
    network/tcpsocket.cpp \
    controller/logincontroller.cpp \
    controller/controlcontroller.cpp \
    measurement/btc/btc_plugin.cpp \
    measurement/upnp/upnp.cpp \
    measurement/upnp/upnp_plugin.cpp \
    measurement/ping/ping_plugin.cpp \
    measurement/ping/ping_definition.cpp \
    measurement/ping/ping.cpp \
    report/reportmodel.cpp \
    controller/reportcontroller.cpp \
    types.cpp \
    units.cpp \
    qmlmodule.cpp \
    controller/controller.cpp \
    log/logger.cpp \
    log/logmodel.cpp \
    controller/configcontroller.cpp \
    measurement/measurementplugin.cpp \
    measurement/http/httpgetrequest.cpp \
    measurement/http/httpgetrequest_definition.cpp

HEADERS += \
    export.h \
    client.h \
    types.h \
    connectiontester.h \
    tests/packettrain.h \
    tests/test.h \
    tests/testinfo.h \
    networkhelper.h \
    tests/natporttimes.h \
    settings.h \
    webrequester.h \
    tests/speedtest.h \
    response.h \
    network/requests/request.h \
    network/requests/userregisterrequest.h \
    network/requests/registerdevicerequest.h \
    network/requests/manualrequest.h \
    network/requests/loginrequest.h \
    network/requests/getconfigrequest.h \
    network/requests/requests.h \
    timing/timingfactory.h \
    timing/timing.h \
    timing/periodictiming.h \
    timing/onofftiming.h \
    timing/immediatetiming.h \
    timing/calendartiming.h \
    scheduler/schedulerstorage.h \
    scheduler/schedulermodel.h \
    scheduler/scheduler.h \
    report/reportstorage.h \
    report/reportscheduler.h \
    report/report.h \
    task/taskvalidator.h \
    task/taskexecutor.h \
    task/task.h \
    task/result.h \
    serializable.h \
    network/networkmanager.h \
    measurement/measurementfactory.h \
    measurement/measurement.h \
    measurement/measurementdefinition.h \
    measurement/btc/btc_mp.h \
    measurement/btc/btc_ma.h \
    measurement/btc/btc_definition.h \
    network/udpsocket.h \
    network/tcpsocket.h \
    controller/logincontroller.h \
    controller/controlcontroller.h \
    log/logger.h \
    measurement/measurementplugin.h \
    measurement/btc/btc_plugin.h \
    measurement/upnp/upnp.h \
    measurement/upnp/upnp_plugin.h \
    measurement/ping/ping_plugin.h \
    measurement/ping/ping_definition.h \
    measurement/ping/ping.h \
    report/reportmodel.h \
    storage/storagepaths.h \
    controller/reportcontroller.h \
    deviceinfo.h \
    units.h \
    qmlmodule.h \
    controller/controller.h \
    log/logmodel.h \
    controller/configcontroller.h \
    measurement/http/httpgetrequest.h \
    measurement/http/httpgetrequest_definition.h

OTHER_FILES += \
    libclient.pri
