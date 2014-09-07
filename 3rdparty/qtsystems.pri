win32 {
    QT += systeminfo
} else {
    INCLUDEPATH += $$PWD/qtsystems_fixes \
                   $$PWD/qtsystems_fixes/QtSystemInfo

    DEFINES += QT_BUILD_SYSTEMINFO_LIB

    # From here on this is mostly copied from systeminfo.pro

    HEADERS += $$PWD/qtsystems/src/systeminfo/qsysteminfoglobal.h \
               $$PWD/qtsystems/src/systeminfo/qdeviceinfo.h \
               $$PWD/qtsystems/src/systeminfo/qstorageinfo.h \
               $$PWD/qtsystems/src/systeminfo/qscreensaver.h \
               $$PWD/qtsystems/src/systeminfo/qbatteryinfo.h \
               $$PWD/qtsystems/src/systeminfo/qnetworkinfo.h

    SOURCES += $$PWD/qtsystems/src/systeminfo/qdeviceinfo.cpp \
               $$PWD/qtsystems/src/systeminfo/qstorageinfo.cpp \
               $$PWD/qtsystems/src/systeminfo/qscreensaver.cpp \
               $$PWD/qtsystems/src/systeminfo/qbatteryinfo.cpp \
               $$PWD/qtsystems/src/systeminfo/qnetworkinfo.cpp

    win32: !simulator: {
        contains(CONFIG, release) {
    #       CONFIG -= console
        }

        win32-msvc*: {
            LIBS += -lUser32 -lGdi32 -lPowrProf -lBthprops -lWs2_32 -lVfw32 -lSetupapi -lIphlpapi -lOle32 -lWbemuuid
        }

        win32-g++*: {
            LIBS += -luser32 -lgdi32 -lpowrprof -lbthprops -lws2_32 -lmsvfw32 -lavicap32 -luuid
        }

        PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/windows/qscreensaver_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qdeviceinfo_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qstorageinfo_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qbatteryinfo_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qnetworkinfo_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qwmihelper_win_p.h \
                           $$PWD/qtsystems/src/systeminfo/windows/qsysteminfoglobal_p.h

        SOURCES += $$PWD/qtsystems/src/systeminfo/windows/qscreensaver_win.cpp \
                   $$PWD/qtsystems/src/systeminfo/windows/qdeviceinfo_win.cpp \
                   $$PWD/qtsystems/src/systeminfo/windows/qstorageinfo_win.cpp \
                   $$PWD/qtsystems/src/systeminfo/windows/qbatteryinfo_win.cpp \
                   $$PWD/qtsystems/src/systeminfo/windows/qnetworkinfo_win.cpp \
                   $$PWD/qtsystems/src/systeminfo/windows/qwmihelper_win.cpp

           LIBS += \
                -lOle32 \
                -lUser32 \
                -lGdi32 \
                -lIphlpapi \
                -lOleaut32 \
                -lPowrProf \
                -lSetupapi

      win32-g++: {
            LIBS += -luser32 -lgdi32
        }

    }

    linux-*: !simulator: {
        PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qdeviceinfo_linux_p.h \
                           $$PWD/qtsystems/src/systeminfo/linux/qstorageinfo_linux_p.h \
                           $$PWD/qtsystems/src/systeminfo/linux/qnetworkinfo_linux_p.h \
                           $$PWD/qtsystems/src/systeminfo/linux/qscreensaver_linux_p.h

        SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qdeviceinfo_linux.cpp \
                   $$PWD/qtsystems/src/systeminfo/linux/qstorageinfo_linux.cpp \
                   $$PWD/qtsystems/src/systeminfo/linux/qnetworkinfo_linux.cpp \
                   $$PWD/qtsystems/src/systeminfo/linux/qscreensaver_linux.cpp

        x11|config_x11: !contains(CONFIG,nox11option) {
            CONFIG += link_pkgconfig
            PKGCONFIG += x11
        } else: {
            DEFINES += QT_NO_X11
        }

        config_bluez {
            CONFIG += link_pkgconfig
            PKGCONFIG += bluez
        } else: {
            DEFINES += QT_NO_BLUEZ
        }

        qtHaveModule(dbus) {
            QT += dbus
            contains(CONFIG,ofono): {
                PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qofonowrapper_p.h
                SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qofonowrapper.cpp
            } else {
                DEFINES += QT_NO_OFONO
            }

            config_udisks {
                QT_PRIVATE += dbus
            } else: {
                DEFINES += QT_NO_UDISKS
            }
            contains(CONFIG,upower): {
                SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qdevicekitservice_linux.cpp \
                           $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_upower.cpp
                HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qdevicekitservice_linux_p.h \
                           $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_upower_p.h
            } else {
                HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_linux_p.h
                SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_linux.cpp

                DEFINES += QT_NO_UPOWER
            }

        } else {
            DEFINES += QT_NO_OFONO QT_NO_UDISKS QT_NO_UPOWER
            HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_linux_p.h
            SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qbatteryinfo_linux.cpp
        }

        config_udev {
            CONFIG += link_pkgconfig
            PKGCONFIG += udev
            LIBS += -ludev
            PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qudevwrapper_p.h
            SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qudevwrapper.cpp
        } else {
            DEFINES += QT_NO_UDEV
        }
    }

    macx:!simulator {
    #CONFIG -= x86_64
    QT += core-private
             OBJECTIVE_SOURCES += $$PWD/qtsystems/src/systeminfo/mac/qbatteryinfo_mac.mm \
                      $$PWD/qtsystems/src/systeminfo/mac/qdeviceinfo_mac.mm \
                      $$PWD/qtsystems/src/systeminfo/mac/qnetworkinfo_mac.mm \
                      $$PWD/qtsystems/src/systeminfo/mac/qscreensaver_mac.mm \
                      $$PWD/qtsystems/src/systeminfo/mac/qstorageinfo_mac.mm

             PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/mac/qbatteryinfo_mac_p.h \
                      $$PWD/qtsystems/src/systeminfo/mac/qdeviceinfo_mac_p.h \
                      $$PWD/qtsystems/src/systeminfo/mac/qnetworkinfo_mac_p.h \
                      $$PWD/qtsystems/src/systeminfo/mac/qscreensaver_mac_p.h \
                      $$PWD/qtsystems/src/systeminfo/mac/qstorageinfo_mac_p.h

             LIBS += -framework SystemConfiguration \
                    -framework Foundation \
                    -framework IOKit  \
                    -framework QTKit \
                    -framework CoreWLAN \
                    -framework CoreLocation \
                    -framework CoreFoundation \
                    -framework ScreenSaver \
                    -framework IOBluetooth \
                    -framework CoreServices \
                    -framework DiskArbitration \
                    -framework ApplicationServices
    }

    simulator {
        QT_PRIVATE += simulator
        DEFINES += QT_SIMULATOR
        PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfodata_simulator_p.h \
                           $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfobackend_simulator_p.h \
                           $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfoconnection_simulator_p.h \
                           $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfo_simulator_p.h


        SOURCES += $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfodata_simulator.cpp \
                   $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfobackend_simulator.cpp \
                   $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfoconnection_simulator.cpp \
                   $$PWD/qtsystems/src/systeminfo/simulator/qsysteminfo_simulator.cpp


        linux-*: {
            PRIVATE_HEADERS += \
                               $$PWD/qtsystems/src/systeminfo/linux/qscreensaver_linux_p.h \
                               $$PWD/qtsystems/src/systeminfo/linux/qstorageinfo_linux_p.h

            SOURCES += \
                       $$PWD/qtsystems/src/systeminfo/linux/qscreensaver_linux.cpp \
                       $$PWD/qtsystems/src/systeminfo/linux/qstorageinfo_linux.cpp

            x11|config_x11 {
                CONFIG += link_pkgconfig
                PKGCONFIG += x11
            } else: {
                DEFINES += QT_NO_X11
            }

            config_bluez {
                CONFIG += link_pkgconfig
                PKGCONFIG += bluez
            } else: {
                DEFINES += QT_NO_BLUEZ
            }

            qtHaveModule(dbus) {
                config_ofono: {
                QT += dbus
                PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qofonowrapper_p.h \
                                   $$PWD/qtsystems/src/systeminfo/linux/qnetworkinfo_linux_p.h

                SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qofonowrapper.cpp \
                           $$PWD/qtsystems/src/systeminfo/linux/qnetworkinfo_linux.cpp
                } else {
                    DEFINES += QT_NO_OFONO
                }
                contains(config_test_udisks, yes): {
                    QT_PRIVATE += dbus
                } else: {
                    DEFINES += QT_NO_UDISKS
                }
            } else {
                DEFINES += QT_NO_OFONO QT_NO_UDISKS
            }

            config_udev {
                CONFIG += link_pkgconfig
                PKGCONFIG += udev
                LIBS += -ludev
                PRIVATE_HEADERS += $$PWD/qtsystems/src/systeminfo/linux/qudevwrapper_p.h
                SOURCES += $$PWD/qtsystems/src/systeminfo/linux/qudevwrapper.cpp
            } else {
                DEFINES += QT_NO_UDEV
            }

        }
    }

    #QMAKE_DOCS = $$PWD/../../doc/config/systeminfo/qtsysteminfo.qdocconf

    HEADERS += $$PUBLIC_HEADERS $$PRIVATE_HEADERS
    #load(qt_module)

    # This must be done after loading qt_module.prf
    config_bluez {
        # bluetooth.h is not standards compliant
        contains(QMAKE_CXXFLAGS, -std=c++0x) {
            QMAKE_CXXFLAGS -= -std=c++0x
            QMAKE_CXXFLAGS += -std=gnu++0x
            CONFIG -= c++11
        }
        c++11 {
            CONFIG -= c++11
            QMAKE_CXXFLAGS += -std=gnu++0x
        }
    }
}
