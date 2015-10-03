TEMPLATE = lib
CONFIG += staticlib

linux {
    INCLUDEPATH += $$PWD/avahi

    HEADERS += \
	#$$PWD/avahi/avahi-compat-libdns_sd/dns_sd.h
    SOURCES += \
	#$$PWD/avahi/avahi-compat-libdns_sd/compat.c
}
