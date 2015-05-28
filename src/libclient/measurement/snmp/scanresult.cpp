#include "scanresult.h"

ScanResult::ScanResult()
{

}

// Add a device to the scan result.
// Or add a community string to an existing device.
void ScanResult::addDevice(const QHostAddress &host, const QString &community, const QString &description)
{
    if (m_deviceTable.contains(host)) {
        SnmpDevice &device = m_deviceTable[host];
        if (! device.community().contains(community)) {
            device.community() << community;
        }
        return;
    }
    SnmpDevice device(host, community, description);
    m_deviceTable.insert(host, device);
}


// SnmpDevice
// Standard Constructor
SnmpDevice::SnmpDevice()
{

}

// SnmpDevice
// Constructor
SnmpDevice::SnmpDevice(const QHostAddress &host, const QString community, const QString description) :
    m_description(description),
    m_host(host)
{
    m_community << community;
}
