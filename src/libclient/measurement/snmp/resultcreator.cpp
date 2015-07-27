#include "resultcreator.h"

// Constructor
ResultCreator::ResultCreator(const QString &gatewayAddr, QObject *parent) :
    QObject(parent),
    m_defaultGatewayAddr(gatewayAddr)
{
    // IMPORTANT: If you changes the order of these list then change it in enum 'MibValues' too.
    m_objectIdList << "ipForwarding.0"
                   << "ifNumber.0"
                   << ".1.3.6.1.2.1.43.5.1.1.1.1"       // Value in Printer-MIB.
                   << ".1.3.6.1.2.1.43.11.1.1.2.1.1";   // Value in Printer-MIB.
}

// SLOT
// Takes a pointer to the scan result and creates the probes result.
// It does a request to each device to distinguish between printer,
// switches and router.
void ResultCreator::createResult(const DeviceMap *scanResult)
{
    m_resultMap.clear();
    foreach (SnmpDevice device, scanResult->deviceList()) {
        QVariantMap deviceMap;
        deviceMap.insert(QString("host"), device.host.toString());
        deviceMap.insert(QString("communityList"), device.m_communityList);
        deviceMap.insert(QString("description"), device.description);
        SnmpPacket packet = sendSnmpRequest(device.host, device.communityName(), m_objectIdList);
        if (packet.isEmpty())
        {
            // Error. Could not communicate with agent. Is defined as 'unknown'.
            deviceMap.insert(QString("type"), QString("unknown"));
            m_resultMap.insert(device.host.toString(), deviceMap);
            continue;
        }
        if (isDeviceRouter(packet))
        {
            deviceMap.insert(QString("type"), QString("Router"));
        }
        else if (isDeviceSwitch(packet))
        {
            deviceMap.insert(QString("type"), QString("Switch"));
        }
        else if (isDevicePrinter(packet))
        {
            deviceMap.insert(QString("type"), QString("Printer"));
        }
        else
        {
            deviceMap.insert(QString("type"), QString("unknown"));
        }

        m_resultMap.insert(device.host.toString(), deviceMap);
    }

    // Identify default gateway
    QVariantMap &device = (QVariantMap&)m_resultMap[m_defaultGatewayAddr];
    if (! device.isEmpty())
    {
        device.insert(QString("type"), QString("Gateway"));
    }

    emit scanResultReady();
}

// Its a printer if at least one of two values in Printer MIB are present.
bool ResultCreator::isDevicePrinter(const SnmpPacket &packet) const
{
    quint8 dataType1 = packet.valueTypeAt(PrinterValueOne);
    quint8 dataType2 = packet.valueTypeAt(PrinterValueTwo);
    // Data type shows if value is not available.
    bool notMib1 = dataType1 == noSuchObject || dataType1 == noSuchInstance || dataType1 == endOfMibView;
    bool notMib2 = dataType2 == noSuchObject || dataType2 == noSuchInstance || dataType2 == endOfMibView;
    if (notMib1 && notMib2)
    {
        // None of the values are available. It is not a printer.
        return false;
    }

    return true;
}

// Its a router if 'ipForwarding.0' is set to 1
// and it has many network interfaces. (more then 3 interfaces)
bool ResultCreator::isDeviceRouter(const SnmpPacket &packet) const
{
    quint8 dataType = packet.valueTypeAt(IpForwarding);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int ipForwarding = packet.intValueAt(IpForwarding);
    dataType = packet.valueTypeAt(InterfaceNumber);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int numInterfaces = packet.intValueAt(InterfaceNumber);

    return ipForwarding == 1 && numInterfaces >= 4;
}

// Takes a response packet.
// Tests if device is a switch. No ipForwarding but many interfaces.
bool ResultCreator::isDeviceSwitch(const SnmpPacket &packet) const
{
    quint8 dataType = packet.valueTypeAt(IpForwarding);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int ipForwarding = packet.intValueAt(IpForwarding);
    dataType = packet.valueTypeAt(InterfaceNumber);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int numInterfaces = packet.intValueAt(InterfaceNumber);

    return ipForwarding != 1 && numInterfaces >= 4;
}

// Send one or more SNMP requests to a devices.
SnmpPacket ResultCreator::sendSnmpRequest(const QHostAddress &host, const QString &community, const QStringList &oidList) const
{
    SnmpPacket packet = SnmpPacket::snmpGetRequest(SNMP_VERSION_1, community);
    snmp_session *ss = packet.getSnmpSession(host);
    if (ss == NULL)
    {
        // Could not open session.
        return SnmpPacket();
    }

    // Set OID's to request.
    foreach (QString objectId, oidList) {
        if (! packet.addNullValue(objectId))
        {
            // Could not set OID.
            return SnmpPacket();
        }
    }

    snmp_pdu *pdu = packet.pduClone();

    struct snmp_pdu *response = NULL;
    int result = snmp_synch_response(ss, pdu, &response);
    snmp_close(ss);
    if (result != STAT_SUCCESS)
    {
        // Error could not communicate with agent.
        return SnmpPacket();
    }

    return SnmpPacket::fromPduStruct(response);
}
