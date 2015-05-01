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
 * connected to a subnet and have an IPv4 address.
 * It takes an interface and calculates the IP range with interfaces
 * IP address and netmask.
 * To send a request to a SNMP agent function 'writteDatagram()' is
 * called. If message was send the signal 'bytesWritten()' is emitted.
 * This signal causes the scanner to send the next request to another
 * IP address.
 * SNMP requests will be send to each IP address in range. If the end
 * of the range is arrived then signal 'retry()' is emitted.
 * These signal calls 'doRetry()'. That will set the scanner back to
 * start scan the IP range again if retries are configured.
 * If retries are done the signal 'changeSnmpCommunity()' is emitted.
 * This calls 'nextSnmpCommunityString()'. Function creates a new UDP
 * datagram with another community string from the list. Scan starts
 * again with the new community string and does its retries.
 * Afterwarts if no more community strings available the next network
 * interface will be selected. And the prozedure starts again until
 * no more interfaces are available.
 *
 * A range of IP addresses
 * Same as automatically detect but will only use the given IP range.
 * Do not search for interfaces.
 */

#include "snmppaket.h"
#include <QUdpSocket>
#include <QNetworkAddressEntry>
#include <QHash>

class SnmpScanner : public QUdpSocket
{
    Q_OBJECT
public:
    explicit SnmpScanner(QObject *parent = 0);
    ~SnmpScanner();

    bool startScan(const long version, const QStringList &communityList, const QString& objectId, const quint8 retriesPerIp);

signals:
    void retry();
    void changeSnmpCommunity();
    void changeInterface();

public slots:
    void sendPaketToNextIP();
    void doRetry();
    void scanNextSnmpCommunity();
    void scanNextInterface();
    void readResponse();

private:
    QString m_objectId;
    long m_snmpVersion;
    const int m_port;
    QList<QNetworkInterface> m_interfaceList;
    quint32 m_firstIp;
    quint32 m_currentIp;
    quint32 m_lastIp;
    QStringList m_communityList;
    quint8 m_retriesPerIp;
    quint8 m_retryCount;
    quint8 m_currentCommunityIndex;
    QByteArray m_datagram;
    QHash<QHostAddress, QPair<QString, QString> > m_resultMap;

    // Methods
    QList<QNetworkInterface> getCurrentlyOnlineInterfacesIPv4() const;
    bool hasInterfaceIPv4Entry(const QNetworkInterface &interface) const;
    QNetworkAddressEntry getInterfacesIPv4Entry(const QNetworkInterface &interface);
    quint32 getInterfacesLowestIPv4(const QNetworkInterface &interface);
    quint32 getInterfacesHighestIPv4(const QNetworkInterface &interface);
    bool nextIp()       { ++m_currentIp; return (m_currentIp <= m_lastIp && m_currentIp > m_firstIp); }
};

#endif // SNMPSCANNER_H
