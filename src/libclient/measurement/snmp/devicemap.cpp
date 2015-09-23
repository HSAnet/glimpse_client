#include "devicemap.h"

// Device Map
// Constructor
DeviceMap::DeviceMap()
{

}

// Inserts a snmp device or if this device exists an community name will be added.
void DeviceMap::addDevice(const SnmpDevice &device)
{
    if (m_deviceMap.contains(device.host))
    {
        SnmpDevice &listedDevice = m_deviceMap[device.host];
        QString communityName = device.firstCommunityName();
        if (!listedDevice.m_communityList.contains(communityName))
        {
            listedDevice.m_communityList << communityName;
        }
    }
    else
    {
        m_deviceMap.insert(device.host, device);
    }
}


// Snmp Device
// Constructor
SnmpDevice::SnmpDevice()
{

}

// Snmp Device
// Constructor
SnmpDevice::SnmpDevice(const QString &description, const QString &community, const QHostAddress &host, const int snmpVersion) :
    description(description),
    host(host),
    snmpVersion(snmpVersion)
{
    m_communityList << community;
}
