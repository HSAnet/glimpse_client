#ifndef RESULTCREATOR_H
#define RESULTCREATOR_H

/* Class ResultCreator
 * -----------------------
 * Takes the list of found devices from the scanner and
 * does some lookup to decide if a device is a printer,
 * switch, router or default gateway.
 * It packs all the gathered information into a QVariantMap
 * with IP address as key.
 */

#include "snmppacket.h"
#include "devicemap.h"
#include <net-snmp/net-snmp-config.h>
#include <net-snmp/net-snmp-includes.h>

class ResultCreator : public QObject
{
    Q_OBJECT
public:
    explicit ResultCreator(QObject *parent = 0);

    void setDefaultGateway(const QString &gateway);
    QVariantMap resultMap() const                   { return m_resultMap; }

signals:
    void creatorResultReady();

public slots:
    void createResult(const DeviceMap *scanResult);
    void createResult(const QVariantList &indexList, const QVariantList &operatingStatusList,
                      const QVariantList &outOctetList, const QVariantList &inOctetList);
    void createResult(const SnmpPacket &response, const QHostAddress &host);

private:
    enum MibValues { IpForwarding, InterfaceNumber, PrinterValueOne, PrinterValueTwo };
    enum IfOperStatus { Up = 1, Down, Testing, Unknown, Dormant, NotPresent, LowerLayerDown };
    QStringList m_objectIdList;
    QVariantMap m_resultMap;
    QString m_defaultGatewayAddr;

    // Methods
    bool isDevicePrinter(const SnmpPacket &packet) const;
    bool isDeviceRouter(const SnmpPacket &packet) const;
    bool isDeviceSwitch(const SnmpPacket &packet) const;
};

#endif // RESULTCREATOR_H
