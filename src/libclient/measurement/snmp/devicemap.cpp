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
        QString communityName = device.communityName();
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
SnmpDevice::SnmpDevice(const QString &description, const QString &community, QHostAddress &host) :
    description(description),
    host(host)
{
    m_communityList << community;
}
