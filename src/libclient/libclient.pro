TEMPLATE = lib
TARGET = client

QT += network concurrent

CONFIG += hide_symbols

android|mac:CONFIG += staticlib

staticlib {
    DEFINES += LIBCLIENT_STATIC
} else {
    DEFINES += LIBCLIENT_BUILD

    win32:LIBS += -lws2_32 -lCrypt32 -lPdh -lntdll
}

# Add the ld.so.conf.d/glimpse.conf file
linux:!android {
    ldconfig.files = glimpse.conf
    ldconfig.path = /etc/ld.so.conf.d/
    INSTALLS += ldconfig
}

INCLUDEPATH += $$PWD

include($$SOURCE_DIRECTORY/dist.pri)
include($$SOURCE_DIRECTORY/3rdparty/miniupnpc.pri)
include($$SOURCE_DIRECTORY/3rdparty/breakpad.pri)
include($$SOURCE_DIRECTORY/3rdparty/winpcap.pri)
include($$SOURCE_DIRECTORY/3rdparty/qtsystems.pri)

android {
    QT += androidextras

    HEADERS += androidhelper.h

    SOURCES += androidhelper.cpp \
               storage/storagepaths_android.cpp \
               log/logger_android.cpp \
               deviceinfo_android.cpp \
               measurement/ping/ping_linux.cpp \
               measurement/wifilookup/wifilookup_android.cpp
} else: ios {
    SOURCES += log/logger_all.cpp \
               measurement/ping/ping_osx.cpp \
               measurement/wifilookup/wifilookup.cpp

    OBJECTIVE_SOURCES += storage/storagepaths_ios.mm \
                         deviceinfo_ios.mm
} else {
    SOURCES += storage/storagepaths.cpp \
               log/logger_all.cpp \
               measurement/wifilookup/wifilookup.cpp

    osx: SOURCES += deviceinfo_osx.cpp \
                    measurement/ping/ping_osx.cpp
    else:unix: SOURCES += deviceinfo_unix.cpp
    else: SOURCES += deviceinfo.cpp \
                     measurement/ping/ping_win.cpp

    linux {
        SOURCES += measurement/ping/ping_linux.cpp
    }
}

breakpad-builtin {
    HEADERS += crashhandler.h
    ios:OBJECTIVE_SOURCES += crashhandler_ios.mm
    else:SOURCES += crashhandler.cpp
}

SOURCES +=  \
    client.cpp \
    connectiontester.cpp \
    networkhelper.cpp \
    settings.cpp \
    webrequester.cpp \
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
    timing/ntp.cpp \
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
    measurement/btc/btc_plugin.cpp \
    measurement/upnp/upnp.cpp \
    measurement/upnp/upnp_plugin.cpp \
    report/reportmodel.cpp \
    controller/reportcontroller.cpp \
    types.cpp \
    units.cpp \
    controller/controller.cpp \
    log/logger.cpp \
    log/logmodel.cpp \
    controller/configcontroller.cpp \
    measurement/measurementplugin.cpp \
    controller/taskcontroller.cpp \
    measurement/packettrains/packettrainsdefinition.cpp \
    measurement/packettrains/packettrains_ma.cpp \
    measurement/packettrains/packettrains_mp.cpp \
    measurement/packettrains/packettrainsplugin.cpp \
    controller/crashcontroller.cpp \
    measurement/http/httpdownload.cpp \
    measurement/http/httpdownload_definition.cpp \
    measurement/http/httpdownload_plugin.cpp \
    timing/ondemandtiming.cpp \
    log/filelogger.cpp \
    measurement/ping/ping_definition.cpp \
    measurement/ping/ping_plugin.cpp \
    measurement/traceroute/traceroute.cpp \
    measurement/traceroute/traceroute_definition.cpp \
    measurement/traceroute/traceroute_plugin.cpp \
    measurement/dnslookup/dnslookup_definition.cpp \
    measurement/dnslookup/dnslookup_plugin.cpp \
    measurement/dnslookup/dnslookup.cpp \
    measurement/reverse_dnslookup/reverseDnslookup.cpp \
    measurement/reverse_dnslookup/reverseDnslookup_definition.cpp \
    measurement/reverse_dnslookup/reverseDnslookup_plugin.cpp \
    channel.cpp \
    network/requests/resourcerequest.cpp \
    network/responses/response.cpp \
    network/responses/loginresponse.cpp \
    network/responses/registerdeviceresponse.cpp \
    network/responses/getconfigresponse.cpp \
    network/responses/getinstructionresponse.cpp \
    network/responses/getscheduleresponse.cpp \
    network/responses/reportresponse.cpp \
    trafficbudgetmanager.cpp \
    precondition.cpp \
    network/requests/uploadrequest.cpp \
    localinformation.cpp \
    measurement/wifilookup/wifilookup_definition.cpp \
    measurement/wifilookup/wifilookup_plugin.cpp

HEADERS += \
    export.h \
    client.h \
    types.h \
    connectiontester.h \
    networkhelper.h \
    settings.h \
    webrequester.h \
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
    timing/ntp.h \
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
    log/logger.h \
    measurement/measurementplugin.h \
    measurement/btc/btc_plugin.h \
    measurement/upnp/upnp.h \
    measurement/upnp/upnp_plugin.h \
    report/reportmodel.h \
    storage/storagepaths.h \
    controller/reportcontroller.h \
    deviceinfo.h \
    units.h \
    controller/controller.h \
    log/logmodel.h \
    controller/configcontroller.h \
    controller/taskcontroller.h \
    measurement/packettrains/packettrainsdefinition.h \
    measurement/packettrains/packettrains_mp.h \
    measurement/packettrains/packettrains_ma.h \
    measurement/packettrains/packettrainsplugin.h \
    controller/crashcontroller.h \
    measurement/http/httpdownload.h \
    measurement/http/httpdownload_definition.h \
    measurement/http/httpdownload_plugin.h \
    timing/ondemandtiming.h \
    log/filelogger.h \
    measurement/ping/ping.h \
    measurement/ping/ping_definition.h \
    measurement/ping/ping_plugin.h \
    measurement/traceroute/traceroute.h \
    measurement/traceroute/traceroute_definition.h \
    measurement/traceroute/traceroute_plugin.h \
    measurement/dnslookup/dnslookup_definition.h \
    measurement/dnslookup/dnslookup_plugin.h \
    measurement/dnslookup/dnslookup.h \
    measurement/reverse_dnslookup/reverseDnslookup.h \
    measurement/reverse_dnslookup/reverseDnslookup_definition.h \
    measurement/reverse_dnslookup/reverseDnslookup_plugin.h \
    channel.h \
    network/requests/resourcerequest.h \
    network/responses/response.h \
    network/responses/loginresponse.h \
    network/responses/registerdeviceresponse.h \
    network/responses/getconfigresponse.h \
    network/responses/getinstructionresponse.h \
    network/responses/getscheduleresponse.h \
    network/responses/reportresponse.h \
    trafficbudgetmanager.h \
    precondition.h \
    network/requests/uploadrequest.h \
    localinformation.h \
    measurement/wifilookup/wifilookup.h \
    measurement/wifilookup/wifilookup_definition.h \
    measurement/wifilookup/wifilookup_plugin.h \
    ident.h

OTHER_FILES += \
    libclient.pri
