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
    explicit ResultCreator(const QString &gatewayAddr, QObject *parent = 0);

    QVariantMap resultMap() const                   { return m_resultMap; }

signals:
    void scanResultReady();

public slots:
    void createResult(const DeviceMap *scanResult);

private:
    enum ImplicitNull { noSuchObject = 128, noSuchInstance, endOfMibView };
    enum MibValues { IpForwarding, InterfaceNumber, PrinterValueOne, PrinterValueTwo };
    QStringList m_objectIdList;
    QVariantMap m_resultMap;
    QString m_defaultGatewayAddr;

    // Methods
    bool isDevicePrinter(const SnmpPacket &packet) const;
    bool isDeviceRouter(const SnmpPacket &packet) const;
    bool isDeviceSwitch(const SnmpPacket &packet) const;
    SnmpPacket sendSnmpRequest(const QHostAddress &host, const QString &community, const QStringList &oidList) const;
};

#endif // RESULTCREATOR_H
