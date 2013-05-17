DEFINES += HAVE_UPNP sun

INCLUDEPATH += $$PWD/miniupnp

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
    $$PWD/miniupnp/miniupnpc/upnpreplyparse.h

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
    $$PWD/miniupnp/miniupnpc/minixmlvalid.c \
    $$PWD/miniupnp/miniupnpc/portlistingparse.c \
    $$PWD/miniupnp/miniupnpc/receivedata.c \
#    $$PWD/miniupnp/miniupnpc/testigddescparse.c \
#    $$PWD/miniupnp/miniupnpc/testminiwget.c \
#    $$PWD/miniupnp/miniupnpc/testminixml.c \
#    $$PWD/miniupnp/miniupnpc/testupnpreplyparse.c \
    $$PWD/miniupnp/miniupnpc/upnpc.c \
    $$PWD/miniupnp/miniupnpc/upnpcommands.c \
    $$PWD/miniupnp/miniupnpc/upnperrors.c \
    $$PWD/miniupnp/miniupnpc/upnpreplyparse.c \
#    $$PWD/miniupnp/miniupnpc/wingenminiupnpcstrings.c
