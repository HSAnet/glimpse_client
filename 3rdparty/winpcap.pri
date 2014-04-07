win32 {
    DEFINES += WPCAP HAVE_REMOTE

    INCLUDEPATH += $$PWD/winpcap

    LIBS += \
        -L$$PWD/winpcap \
        -lwpcap \
        -lpacket

    HEADERS += \
        $$PWD/winpcap/pcap.h
}
