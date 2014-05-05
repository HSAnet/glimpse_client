TEMPLATE = lib
TARGET = client

QT += network concurrent

CONFIG += hide_symbols

android|osx:CONFIG += staticlib

staticlib {
    DEFINES += LIBCLIENT_STATIC
} else {
    DEFINES += LIBCLIENT_BUILD

    win32:LIBS += -lws2_32 -lCrypt32 -lPdh
}

# Add the ld.so.conf.d/glimpse.conf file
linux:!android {
    ldconfig.files = glimpse.conf
    ldconfig.path = /etc/ld.so.conf.d/
    INSTALLS += ldconfig
}

INCLUDEPATH += $$PWD

include(../../dist.pri)
include(../../3rdparty/miniupnpc.pri)
include(../../3rdparty/breakpad.pri)
include(../../3rdparty/winpcap.pri)

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
               androidprocessmodel.cpp \
               measurement/udpping/udpping_linux.cpp
} else: ios {
    SOURCES += log/logger_all.cpp
    OBJECTIVE_SOURCES += storage/storagepaths_ios.mm \
                         deviceinfo_ios.mm
} else {
    SOURCES += storage/storagepaths.cpp \
               log/logger_all.cpp

    osx: SOURCES += deviceinfo_osx.cpp \
                    measurement/udpping/udpping.cpp
    else:unix: SOURCES += deviceinfo_unix.cpp
    else: SOURCES += deviceinfo.cpp \
                     measurement/udpping/udpping_win.cpp

    linux {
        HEADERS += linuxprocessmodel.h
        SOURCES += linuxprocessmodel.cpp \
                   measurement/udpping/udpping_linux.cpp

        qtHaveModule(quick) {
            CONFIG += link_pkgconfig
            PKGCONFIG += libwnck-1.0

            HEADERS += linuximageprovider.h
            SOURCES += linuximageprovider.cpp
        }
    }
}

osx {
    QT += macextras
    LIBS += -framework AppKit -framework DiskArbitration

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

qtHaveModule(quick) {
    QT += qml quick
    HEADERS += qmlmodule.h
    SOURCES += qmlmodule.cpp
}

SOURCES +=  \
    client.cpp \
    connectiontester.cpp \
    networkhelper.cpp \
    settings.cpp \
    webrequester.cpp \
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
    measurement/ping/ping_plugin.cpp \
    measurement/ping/ping_definition.cpp \
    measurement/ping/ping.cpp \
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
    network/requests/gettasksrequest.cpp \
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
    measurement/udpping/udpping_definition.cpp \
    measurement/udpping/udpping_plugin.cpp \
    measurement/traceroute/traceroute.cpp \
    measurement/traceroute/traceroute_definition.cpp \
    measurement/traceroute/traceroute_plugin.cpp

HEADERS += \
    export.h \
    client.h \
    types.h \
    connectiontester.h \
    networkhelper.h \
    settings.h \
    webrequester.h \
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
    measurement/ping/ping_plugin.h \
    measurement/ping/ping_definition.h \
    measurement/ping/ping.h \
    report/reportmodel.h \
    storage/storagepaths.h \
    controller/reportcontroller.h \
    deviceinfo.h \
    units.h \
    controller/controller.h \
    log/logmodel.h \
    controller/configcontroller.h \
    controller/taskcontroller.h \
    network/requests/gettasksrequest.h \
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
    measurement/udpping/udpping.h \
    measurement/udpping/udpping_definition.h \
    measurement/udpping/udpping_plugin.h \
    measurement/traceroute/traceroute.h \
    measurement/traceroute/traceroute_definition.h \
    measurement/traceroute/traceroute_plugin.h

OTHER_FILES += \
    libclient.pri
