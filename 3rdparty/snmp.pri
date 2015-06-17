# Link SNMP library

macx: {
    LIBS += -L$$PWD/snmp/lib-mac/ -lnetsnmp.30
    INCLUDEPATH += $$PWD/snmp/mac-conf
}
unix:!macx {
    LIBS += -L$$PWD/snmp/lib-linux/ -lnetsnmp
    INCLUDEPATH += $$PWD/snmp/linux-conf
    PRE_TARGETDEPS += $$PWD/snmp/lib-linux/libnetsnmp.a
}
win: {
    LIBS += -L$$PWD/snmp/lib-win/ -lnetsnmp
    INCLUDEPATH += $$PWD/snmp/win-conf
}

INCLUDEPATH += $$PWD/snmp/include
DEPENDPATH += $$PWD/snmp/include
