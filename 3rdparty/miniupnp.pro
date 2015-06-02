TEMPLATE = lib
CONFIG += staticlib
CONFIG -= qt

!win32 {
    strings.target = miniupnp/miniupnpc/miniupnpcstrings.h
    strings.commands = cd $$PWD/miniupnp/miniupnpc && sh updateminiupnpcstrings.sh
    strings.depends = FORCE

    QMAKE_EXTRA_TARGETS += strings
    PRE_TARGETDEPS += miniupnp/miniupnpc/miniupnpcstrings.h
}

DEFINES += HAVE_UPNP MINIUPNP_STATICLIB

# Disable warnings
*clang*|*g++*|*llvm* {
    QMAKE_CFLAGS += -w
    QMAKE_CXXFLAGS += -w
}

android:DEFINES += sun # Atleast needed for android
else:linux:DEFINES += _GNU_SOURCE
mac:DEFINES += _DARWIN_C_SOURCE
win32:DEFINES += _CRT_SECURE_NO_WARNINGS

android|mac|win32|linux {
    INCLUDEPATH += $$PWD/miniupnp
    win32:INCLUDEPATH += $$PWD/miniupnp_windows_fixes

    HEADERS += \
        $$PWD/miniupnp/miniupnpc/bsdqueue.h \
        $$PWD/miniupnp/miniupnpc/codelength.h \
        $$PWD/miniupnp/miniupnpc/connecthostport.h \
        $$PWD/miniupnp/miniupnpc/declspec.h \
        $$PWD/miniupnp/miniupnpc/igd_desc_parse.h \
        $$PWD/miniupnp/miniupnpc/minisoap.h \
        $$PWD/miniupnp/miniupnpc/minissdpc.h \
        $$PWD/miniupnp/miniupnpc/miniupnpc.h \
        $$PWD/miniupnp/miniupnpc/miniupnpctypes.h \
        $$PWD/miniupnp/miniupnpc/miniwget.h \
        $$PWD/miniupnp/miniupnpc/minixml.h \
        $$PWD/miniupnp/miniupnpc/portlistingparse.h \
        $$PWD/miniupnp/miniupnpc/receivedata.h \
        $$PWD/miniupnp/miniupnpc/upnpcommands.h \
        $$PWD/miniupnp/miniupnpc/upnperrors.h \
        $$PWD/miniupnp/miniupnpc/upnpreplyparse.h \
#        $$PWD/miniupnp/miniupnpd/portinuse.h \
#        $$PWD/miniupnp/miniupnpd/upnpurns.h \
#        $$PWD/miniupnp/miniupnpd/config.h \
#        $$PWD/miniupnp/miniupnpd/upnpsoap.h \
#        $$PWD/miniupnp/miniupnpd/minixml.h \
#        $$PWD/miniupnp/miniupnpd/upnpreplyparse.h \
#        $$PWD/miniupnp/miniupnpd/pcpserver.h \
#        $$PWD/miniupnp/miniupnpd/asyncsendto.h \
#        $$PWD/miniupnp/miniupnpd/upnppinhole.h \
#        $$PWD/miniupnp/miniupnpd/upnphttp.h \
#        $$PWD/miniupnp/miniupnpd/upnpredirect.h \
#        $$PWD/miniupnp/miniupnpd/options.h \
#        $$PWD/miniupnp/miniupnpd/macros.h \
#        $$PWD/miniupnp/miniupnpd/getifaddr.h \
#        $$PWD/miniupnp/miniupnpd/upnpdescgen.h \
#        $$PWD/miniupnp/miniupnpd/miniupnpdpath.h \
#        $$PWD/miniupnp/miniupnpd/getroute.h \
#        $$PWD/miniupnp/miniupnpd/pcp_msg_struct.h \
#        $$PWD/miniupnp/miniupnpd/upnpglobalvars.h \
#        $$PWD/miniupnp/miniupnpd/upnpevents.h \
#        $$PWD/miniupnp/miniupnpd/ipf/ipfrdr.h \
#        $$PWD/miniupnp/miniupnpd/getifstats.h \
#        $$PWD/miniupnp/miniupnpd/upnputils.h \
#        $$PWD/miniupnp/miniupnpd/pcplearndscp.h \
#        $$PWD/miniupnp/miniupnpd/codelength.h \
#        $$PWD/miniupnp/miniupnpd/commonrdr.h \
#        $$PWD/miniupnp/miniupnpd/natpmp.h \
#        $$PWD/miniupnp/miniupnpd/daemonize.h \
#        $$PWD/miniupnp/miniupnpd/ipfw/ipfwaux.h \
#        $$PWD/miniupnp/miniupnpd/ipfw/ipfwrdr.h \
#        $$PWD/miniupnp/miniupnpd/minissdp.h \
#        $$PWD/miniupnp/miniupnpd/upnpdescstrings.h \
#        $$PWD/miniupnp/miniupnpd/pf/pfpinhole.h \
#        $$PWD/miniupnp/miniupnpd/pf/obsdrdr.h \
#        $$PWD/miniupnp/miniupnpd/getconnstatus.h \
#        $$PWD/miniupnp/miniupnpd/upnppermissions.h \
#        $$PWD/miniupnp/miniupnpd/netfilter/iptpinhole.h \
#        $$PWD/miniupnp/miniupnpd/netfilter/iptcrdr.h \
#        $$PWD/miniupnp/miniupnpd/netfilter/tiny_nf_nat.h \
#        $$PWD/miniupnp/miniupnpd/miniupnpdtypes.h \
#        $$PWD/miniupnp/miniupnpd/ifacewatcher.h


    SOURCES += \
        $$PWD/miniupnp/miniupnpc/connecthostport.c \
        $$PWD/miniupnp/miniupnpc/igd_desc_parse.c \
    #    $$PWD/miniupnp/miniupnpc/minihttptestserver.c \
        $$PWD/miniupnp/miniupnpc/minisoap.c \
        $$PWD/miniupnp/miniupnpc/minissdpc.c \
        $$PWD/miniupnp/miniupnpc/miniupnpc.c \
    #    $$PWD/miniupnp/miniupnpc/miniupnpcmodule.c \
        $$PWD/miniupnp/miniupnpc/miniwget.c \
        $$PWD/miniupnp/miniupnpc/minixml.c \
    #    $$PWD/miniupnp/miniupnpc/minixmlvalid.c \
        $$PWD/miniupnp/miniupnpc/portlistingparse.c \
        $$PWD/miniupnp/miniupnpc/receivedata.c \
    #    $$PWD/miniupnp/miniupnpc/testigddescparse.c \
    #    $$PWD/miniupnp/miniupnpc/testminiwget.c \
    #    $$PWD/miniupnp/miniupnpc/testminixml.c \
    #    $$PWD/miniupnp/miniupnpc/testupnpreplyparse.c \
    #    $$PWD/miniupnp/miniupnpc/upnpc.c \
        $$PWD/miniupnp/miniupnpc/upnpcommands.c \
        $$PWD/miniupnp/miniupnpc/upnperrors.c \
        $$PWD/miniupnp/miniupnpc/upnpreplyparse.c \
    #    $$PWD/miniupnp/miniupnpc/wingenminiupnpcstrings.c \
#        $$PWD/miniupnp/miniupnpd/testupnpdescgen.c \
#        $$PWD/miniupnp/miniupnpd/ipf/ipfrdr.c \
#        $$PWD/miniupnp/miniupnpd/ipf/testipfrdr.c \
#        $$PWD/miniupnp/miniupnpd/testgetroute.c \
#        $$PWD/miniupnp/miniupnpd/testportinuse.c \
#        $$PWD/miniupnp/miniupnpd/upnppermissions.c \
#        $$PWD/miniupnp/miniupnpd/upnpredirect.c \
#        $$PWD/miniupnp/miniupnpd/options.c \
#        $$PWD/miniupnp/miniupnpd/daemonize.c \
#        $$PWD/miniupnp/miniupnpd/pcplearndscp.c \
#        $$PWD/miniupnp/miniupnpd/ipfw/ipfwrdr.c \
#        $$PWD/miniupnp/miniupnpd/ipfw/testipfwrdr.c \
#        $$PWD/miniupnp/miniupnpd/ipfw/ipfwaux.c \
#        $$PWD/miniupnp/miniupnpd/testasyncsendto.c \
#        $$PWD/miniupnp/miniupnpd/upnpevents.c \
#        $$PWD/miniupnp/miniupnpd/pf/testpfpinhole.c \
#        $$PWD/miniupnp/miniupnpd/pf/pfpinhole.c \
#        $$PWD/miniupnp/miniupnpd/pf/obsdrdr.c \
#        $$PWD/miniupnp/miniupnpd/pf/testobsdrdr.c \
#        $$PWD/miniupnp/miniupnpd/miniupnpdctl.c \
#        $$PWD/miniupnp/miniupnpd/natpmp.c \
#        $$PWD/miniupnp/miniupnpd/asyncsendto.c \
#        $$PWD/miniupnp/miniupnpd/miniupnpd.c \
#        $$PWD/miniupnp/miniupnpd/solaris/getifstats.c \
#        $$PWD/miniupnp/miniupnpd/getconnstatus.c \
#        $$PWD/miniupnp/miniupnpd/upnphttp.c \
#        $$PWD/miniupnp/miniupnpd/upnppinhole.c \
#        $$PWD/miniupnp/miniupnpd/mac/testgetifstats.c \
#        $$PWD/miniupnp/miniupnpd/mac/getifstats.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/iptcrdr.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/testiptpinhole.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/iptpinhole.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/nfct_get.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/testiptcrdr_peer.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/testiptcrdr_dscp.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/testiptcrdr.c \
#        $$PWD/miniupnp/miniupnpd/netfilter/test_nfct_get.c \
#        $$PWD/miniupnp/miniupnpd/linux/getroute.c \
#        $$PWD/miniupnp/miniupnpd/linux/ifacewatcher.c \
#        $$PWD/miniupnp/miniupnpd/linux/getifstats.c
}

