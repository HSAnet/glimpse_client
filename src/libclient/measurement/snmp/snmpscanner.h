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
 * ___Automatically detect subnets___
 * The scanner searches for network interfaces which are currently
 * connected to a subnet and have an IPv4 address.
 * It takes an interface and calculates the IP range with interfaces
 * IP address and netmask.
 * To send a request to a SNMP agent function 'writteDatagram()' is
 * called. A timer is startet with an interval of x milliseconds.
 * Function 'timerEvent()' is called when time is up. This function
 * calls 'writeDatagram()' again to send a packet to the next ip.
 * Or if all packets are send scanner exits with signal 'scanFinished()'.
 *
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
 * If all interfaces are scanned then the signal 'scanFinished()'
 * is emitted. These signal takes a pointer to the scan result table.
 *
 * ___A range of IP addresses___
 * Same as automatically detect but will only use the given IP range.
 * Do not search for interfaces.
 *
 * The memory of the scan result pointer is handled here. Do NOT free
 * memory of this pointer.
 */

#include "snmppacket.h"
#include "devicemap.h"
#include <QUdpSocket>
#include <QNetworkAddressEntry>


class SnmpScanner : public QUdpSocket
{
    Q_OBJECT
public:
    explicit SnmpScanner(QObject *parent = 0);
    ~SnmpScanner();

    bool startScan(const long version, const QStringList &communityList, const QString& objectId, const quint8 retriesPerIp,
                   const int sendInterval, const int waitTime);
    bool scanRange(const long version, const QStringList &communityList, const QString &objectId, const quint8 retriesPerIp,
                   const QHostAddress &start, const QHostAddress &end, const int sendInterval, const int waitTime);

    // Getter
    QString errorMessage()              { return m_errorMessage; }
    bool stopScanner();

signals:
    void retry();
    void changeSnmpCommunity();
    void changeInterface();
    void scanFinished(const DeviceMap *resultTable);
    void reportError(const QString errorMsg);

private slots:
    void doRetry();
    void scanNextSnmpCommunity();
    void scanNextInterface();
    void readResponse();
    void handleError(QAbstractSocket::SocketError socketError);

private:
    QString m_objectId;     // SNMP Object Identifier
    long m_snmpVersion;
    const int m_port;
    QList<QNetworkInterface> m_interfaceList;
    quint32 m_firstIp;
    quint32 m_currentIp;
    quint32 m_lastIp;
    QStringList m_communityList;
    quint8 m_retriesPerIp;
    quint8 m_currentRetry;
    quint8 m_currentCommunityIndex;
    QByteArray m_datagram;
    DeviceMap *m_pScanResult;
    int m_timerId;
    int m_sendInterval;
    bool m_sentAllPackets;
    QString m_errorMessage;
    int m_waitTime;

    // Methods
    QList<QNetworkInterface> getCurrentlyOnlineInterfacesIPv4() const;
    bool hasInterfaceIPv4Entry(const QNetworkInterface &netAdapter) const;
    QNetworkAddressEntry getInterfacesIPv4Entry(const QNetworkInterface &netAdapter);
    quint32 getInterfacesLowestIPv4(const QNetworkInterface &netAdapter);
    quint32 getInterfacesHighestIPv4(const QNetworkInterface &netAdapter);
    bool nextIp()       { ++m_currentIp; return (m_currentIp <= m_lastIp && m_currentIp > m_firstIp); }
    void timerEvent(QTimerEvent *event);
};

#endif // SNMPSCANNER_H
