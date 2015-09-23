#include "resultcreator.h"

// Constructor
ResultCreator::ResultCreator(QObject *parent) :
    QObject(parent)
{
    // IMPORTANT: If you changes the order of these list then change it in enum 'MibValues' too.
    m_objectIdList << "ipForwarding.0"
                   << "ifNumber.0"
                   << ".1.3.6.1.2.1.43.5.1.1.1.1"       // Value in Printer-MIB.
                   << ".1.3.6.1.2.1.43.11.1.1.2.1.1";   // Value in Printer-MIB.
}

// Setter
void ResultCreator::setDefaultGateway(const QString &gateway)
{
    m_defaultGatewayAddr = gateway;
}

// SLOT
// Takes a pointer to the scan result and creates the probes result.
// It does a request to each device to distinguish between printer,
// switches and router. It searches in scan result for the default
// gateway and marks the device in the result as gateway.
void ResultCreator::createResult(const DeviceMap *scanResult)
{
    m_resultMap.clear();
    SnmpPacket request;
    foreach (SnmpDevice device, scanResult->deviceList()) {
        QString host = device.host.toString();
        QVariantMap deviceMap;
        deviceMap.insert(QString("host"), host);
        deviceMap.insert(QString("communityList"), device.m_communityList);
        deviceMap.insert(QString("description"), device.description);
        deviceMap.insert(QString("snmpVersion"), device.snmpVersion);
        SnmpPacket response = request.synchRequestGet(host, device.snmpVersion, device.firstCommunityName(), m_objectIdList);
        if (response.hasError())
        {
            // Error. Could not communicate with agent. Is defined as 'unknown'.
            deviceMap.insert(QString("type"), QString("unknown"));
            m_resultMap.insert(device.host.toString(), deviceMap);
            continue;
        }
        if (isDeviceRouter(response))
        {
            deviceMap.insert(QString("type"), QString("Router"));
        }
        else if (isDeviceSwitch(response))
        {
            deviceMap.insert(QString("type"), QString("Switch"));
        }
        else if (isDevicePrinter(response))
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
    // Has map (scan result) a device with the IP of the default gateway?
    QVariantMap &device = (QVariantMap&)m_resultMap[m_defaultGatewayAddr];
    if (! device.isEmpty())
    {
        device.insert(QString("type"), QString("Gateway"));
    }

    emit creatorResultReady();
}

// Create Gateway statistics result.
// Stores of all interfaces with status 'up' the out going octets, in going octets and the index.
void ResultCreator::createResult(const QVariantList &indexList, const QVariantList &operatingStatusList,
                                 const QVariantList &outOctetList, const QVariantList &inOctetList)
{
    m_resultMap.clear();
    for(int index=0; index<indexList.size(); ++index)
    {
        if (operatingStatusList[index].toInt() == Up)
        {
            QVariantMap interfaceInfo;
            interfaceInfo.insert(QString("index"), indexList[index]);
            interfaceInfo.insert(QString("outOctets"), outOctetList[index]);
            interfaceInfo.insert(QString("inOctets"), inOctetList[index]);
            m_resultMap.insert(indexList[index].toString(), interfaceInfo);
        }
    }

    emit creatorResultReady();
}

// Create a result map of a response to a SingleRequest.
void ResultCreator::createResult(const SnmpPacket &response, const QString &host)
{
    m_resultMap.clear();
    m_resultMap.insert(QString("host"), host);
    m_resultMap.insert(QString("valueList"), response.oidValueList());

    emit creatorResultReady();
}

// Its a printer if at least one of two values in Printer MIB are present.
bool ResultCreator::isDevicePrinter(const SnmpPacket &packet) const
{
    if (packet.hasPduValueAt(PrinterValueOne) || packet.hasPduValueAt(PrinterValueTwo))
    {
        return true;
    }

    // None of the values are available. It is not a printer.
    return false;
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
