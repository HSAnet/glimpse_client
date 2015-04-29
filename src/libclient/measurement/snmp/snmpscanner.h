#ifndef SNMPSCANNER_H
#define SNMPSCANNER_H

/* ------------------------------------------------------------
 * Clas SnmpScanner
 * ------------------------------------------------------------
 *
 * Scanner has different modes. It can automatically detect all
 * to the device connected subnets and scan there for SNMP agents.
 * Or it can scan a given IP range.
 *
 * Automatically detect subnets
 * The scanner searches for network interfaces which are currently
 * connected to a subnet and have an IPv4 address. It calculates
 * the IP range from interfaces IP address and netmask.
 * SNMP requests will be send on each detected interfcace.
 * To every IP of subnet will be send one request. After the last IP
 * in range is arrived the scanner will start again if more then one
 * retries are set. If all retries are done the scanner will take
 * the next community string from its list and start again with that
 * community string.
 * The scan is over if all community string are used on each interface.
 *
 * A range of IP addresses
 * Same as automatically detect but will only use the given IP range.
 * Do not search for interfaces.
 */

#include "snmppaket.h"
#include <QUdpSocket>
#include <QNetworkAddressEntry>

class SnmpScanner : public QUdpSocket
{
    Q_OBJECT
public:
    explicit SnmpScanner(QObject *parent = 0);
    ~SnmpScanner();

    bool startScan(const QStringList &communityList, const quint8 retriesPerIp);

signals:
    void retry();

public slots:
    void sendPaketToNextIP();
    void doRetry();

private:
    QList<QNetworkInterface> m_interfaceList;
    quint32 m_firstIp;
    quint32 m_currentIp;
    quint32 m_lastIp;
    QStringList m_communityList;
    quint8 m_retriesPerIp;
    quint8 m_retryCount;
    QByteArray m_datagram;

    // Methods
    QList<QNetworkInterface> getCurrentlyOnlineInterfacesIPv4() const;
    bool hasInterfaceIPv4Entry(const QNetworkInterface &interface) const;
    QNetworkAddressEntry getInterfacesIPv4Entry(const QNetworkInterface &interface);
    quint32 getInterfacesLowestIPv4(const QNetworkInterface &interface);
    quint32 getInterfacesHighestIPv4(const QNetworkInterface &interface);
    bool nextIp()       { ++m_currentIp; return (m_currentIp <= m_lastIp && m_currentIp > m_firstIp); }
};

#endif // SNMPSCANNER_H
