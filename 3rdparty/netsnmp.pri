LIBS += -L$$BUILD_DIRECTORY/3rdparty/netsnmp -lnetsnmp

mac: INCLUDEPATH += $$PWD/netsnmp/mac-config
linux: INCLUDEPATH += $$PWD/netsnmp/debian-config

INCLUDEPATH += $$PWD/netsnmp/include
DEPENDPATH += $$PWD/netsnmp/include

PRE_TARGETDEPS += $$BUILD_DIRECTORY/3rdparty/netsnmp/libnetsnmp.a
