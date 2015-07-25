LIBS += -L$$BUILD_DIRECTORY/3rdparty/netsnmp -lnetsnmp

INCLUDEPATH += $$PWD/netsnmp/include \
                $$PWD/netsnmp/mac-config
DEPENDPATH += $$PWD/netsnmp/include

PRE_TARGETDEPS += $$BUILD_DIRECTORY/3rdparty/netsnmp/libnetsnmp.a
