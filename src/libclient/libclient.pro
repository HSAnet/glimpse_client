TEMPLATE = lib
TARGET = client
CONFIG += staticlib

QT += network concurrent qml

staticlib:DEFINES += LIBCLIENT_STATIC
else:DEFINES += LIBCLIENT_BUILD

INCLUDEPATH += $$PWD

include(../../3rdparty/miniupnpc.pri)

SOURCES +=  \
    client.cpp \
    discovery.cpp \
    connectiontester.cpp \
    testfactory.cpp \
    tests/packettrain.cpp \
    testscheduler.cpp \
    networkhelper.cpp \
    tests/natporttimes.cpp \
    settings.cpp \
    webrequester.cpp \
    ping.cpp \
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
    log/logger.cpp \
    measurement/btc/btc_plugin.cpp

HEADERS += \
    export.h \
    client.h \
    types.h \
    discovery.h \
    connectiontester.h \
    testfactory.h \
    tests/packettrain.h \
    tests/test.h \
    testscheduler.h \
    tests/testinfo.h \
    networkhelper.h \
    tests/natporttimes.h \
    settings.h \
    webrequester.h \
    ping.h \
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
    measurement/btc/btc_plugin.h

OTHER_FILES += \
    libclient.pri
