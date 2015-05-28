#ifndef SCANRESULT_H
#define SCANRESULT_H

#include <QHash>
#include <QHostAddress>



class SnmpDevice
{
public:
    SnmpDevice();
    SnmpDevice(const QHostAddress &host, const QString community, const QString description);

    QStringList community() const       { return m_community; }
    QString description() const         { return m_description; }
    QHostAddress host() const           { return m_host; }

private:
    QStringList m_community;
    QString m_description;
    QHostAddress m_host;
};



class ScanResult
{
public:
    ScanResult();

    void addDevice(const QHostAddress &host, const QString &community, const QString &description);
    int count() const                           { return m_deviceTable.count(); }
    QList<SnmpDevice> deviceList() const        { return m_deviceTable.values(); }

private:
    QHash<QHostAddress, SnmpDevice> m_deviceTable;
};

#endif // SCANRESULT_H
