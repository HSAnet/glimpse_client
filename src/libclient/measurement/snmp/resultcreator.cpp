#include "resultcreator.h"

// Constructor
ResultCreator::ResultCreator(QObject *parent) : QObject(parent)
{
    m_objectIdList << "ipForwarding.0"
                   << "ifNumber.0"
                   << ".1.3.6.1.2.1.43.5.1.1.1.1"       // Value in Printer-MIB.
                   << ".1.3.6.1.2.1.43.11.1.1.2.1.1";   // Value in Printer-MIB.
}

// SLOT
// Takes a pointer to the scan result and creates the probes result.
void ResultCreator::createResult(const DeviceMap *scanResult)
{
    qDebug() << "Scanner found " << scanResult->count() << " devices.";
    QVariantMap resultMap;
    foreach (SnmpDevice device, scanResult->deviceList()) {
        QVariantMap deviceMap;
        deviceMap.insert(QString("host"), device.host.toString());
        deviceMap.insert(QString("communityList"), device.m_communityList);
        deviceMap.insert(QString("description"), device.description);
        SnmpPacket packet = sendSnmpRequest(device.host, device.communityName(), m_objectIdList);
        if (packet.isEmpty())
        {
            // Error. Could not communicate with agent.
            deviceMap.insert(QString("type"), QString("unknown"));
            resultMap.insert(device.host.toString(), deviceMap);
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

        resultMap.insert(device.host.toString(), deviceMap);
    }

    qDebug() << "Found " << resultMap.size() << " devices.";
    printResultMap(resultMap);
}

// Its a printer if the third value in response is present.
bool ResultCreator::isDevicePrinter(const SnmpPacket &packet) const
{
    quint8 dataType1 = packet.valueTypeAt(2);
    quint8 dataType2 = packet.valueTypeAt(3);
    bool notMib1 = dataType1 == noSuchObject || dataType1 == noSuchInstance || dataType1 == endOfMibView;
    bool notMib2 = dataType2 == noSuchObject || dataType2 == noSuchInstance || dataType2 == endOfMibView;
    if (notMib1 && notMib2)
    {
        return false;
    }

    return true;
}

// Send a request to the device. Query a value in the IP-MIB.
// Its a router if 'ipForwarding.0' is set to 1.
bool ResultCreator::isDeviceRouter(const SnmpPacket &packet) const
{
    quint8 dataType = packet.valueTypeAt(0);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int ipForwarding = packet.intValueAt(0);
    dataType = packet.valueTypeAt(1);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int numInterfaces = packet.intValueAt(1);

    return ipForwarding == 1 && numInterfaces >= 4;
}

// Takes the response of a request (ipForwarding and ifNumber).
// Tests if device is a switch. No forwarding but many interfaces.
bool ResultCreator::isDeviceSwitch(const SnmpPacket &packet) const
{
    quint8 dataType = packet.valueTypeAt(0);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int ipForwarding = packet.intValueAt(0);
    dataType = packet.valueTypeAt(1);
    if (dataType != ASN_INTEGER)
    {
        return false;
    }
    int numInterfaces = packet.intValueAt(1);

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

// DEBUG
// Print scann result.
void ResultCreator::printResultMap(const QVariantMap &resultMap) const
{
    QTextStream outStream(stdout);
    foreach ( QVariant value, resultMap.values()) {
        QVariantMap map = value.value<QVariantMap>();
        outStream << "----------------------------------------------------" << endl;
        outStream << "Host              : " << map.value("host").toString() << endl;
        outStream << "Community list    : ";
        foreach (QString communityName, map.value("communityList").toStringList()) {
            outStream << communityName << " ";
        }
        outStream << endl;
        outStream << "Description       : " << map.value("description").toString() << endl;
        outStream << "Type              : " << map.value("type").toString() << endl;
    }
    outStream << "----------------------------------------------------" << endl;
}

