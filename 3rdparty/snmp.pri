# ---------------------------------
# Net - SNMP library
#

INCLUDEPATH += $$PWD/snmp/include

macx {
    LIBS += $$PWD/snmp/lib -lnetsnmp
}
