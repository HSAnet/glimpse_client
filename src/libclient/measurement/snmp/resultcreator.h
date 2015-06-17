#ifndef RESULTCREATOR_H
#define RESULTCREATOR_H

#include "snmppacket.h"
#include "devicemap.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

class ResultCreator : public QObject
{
    Q_OBJECT
public:
    explicit ResultCreator(QObject *parent = 0);

signals:

public slots:
    void createResult(const DeviceMap *scanResult);

private:
    enum ImplicitNull { noSuchObject = 128, noSuchInstance, endOfMibView };
    QStringList m_objectIdList;

    // Methods
    bool isDevicePrinter(const SnmpPacket &packet) const;
    bool isDeviceRouter(const SnmpPacket &packet) const;
    bool isDeviceSwitch(const SnmpPacket &packet) const;
    SnmpPacket sendSnmpRequest(const QHostAddress &host, const QString &community, const QStringList &oidList) const;

    // Debug methods
    void printResultMap(const QVariantMap &resultMap) const;
};

#endif // RESULTCREATOR_H
