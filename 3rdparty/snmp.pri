# Link SNMP library

mac: {
    LIBS += -L$$PWD/snmp/lib-mac/ -lnetsnmp.30
    INCLUDEPATH += $$PWD/snmp/mac-conf
}
unix: {
    LIBS += -L$$PWD/snmp/lib-linux/ -lnetsnmp.30
    INCLUDEPATH += $$PWD/snmp/linux-conf
}

INCLUDEPATH += $$PWD/snmp/include
DEPENDPATH += $$PWD/snmp/include
