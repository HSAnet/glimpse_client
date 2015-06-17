#ifndef DEVICEMAP_H
#define DEVICEMAP_H

#include <QHostAddress>
#include <QVariantMap>

// Class
// Represents a snmp device
class SnmpDevice
{
public:
    SnmpDevice();
    SnmpDevice(const QString &description, const QString &m_communityList, QHostAddress &host);

    QString communityName() const           { return m_communityList[0]; }

    // Members
    QString description;
    QStringList m_communityList;
    QHostAddress host;
};


// Class
// Map to store found snmp devices.
class DeviceMap
{
public:
    DeviceMap();

    void addDevice(const SnmpDevice &device);

    // Inline functions
    void clear()                            { m_deviceMap.clear(); }
    int count() const                       { return m_deviceMap.count(); }
    QList<SnmpDevice> deviceList() const      { return m_deviceMap.values(); }

private:
    QHash<QHostAddress, SnmpDevice> m_deviceMap;
};

#endif // DEVICEMAP_H
