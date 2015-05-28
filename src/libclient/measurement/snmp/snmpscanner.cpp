#include "snmpscanner.h"

// Constructor
SnmpScanner::SnmpScanner(QObject *parent)
    : QUdpSocket(parent),
      m_port(161),
      m_pResultTable(new ScanResult()),
      m_sendInterval(2),
      m_sentAllPackets(false)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readResponse()));
    connect(this, SIGNAL(retry()), this, SLOT(doRetry()));
    connect(this, SIGNAL(changeSnmpCommunity()), SLOT(scanNextSnmpCommunity()));
    connect(this, SIGNAL(changeInterface()), this, SLOT(scanNextInterface()));
}
// Destructor
SnmpScanner::~SnmpScanner()
{
}

// Start SNMP device scan. Send UDP datagram to each ip in available subnets.
bool SnmpScanner::startScan(const long version, const QStringList &communityList, const QString &objectId, const quint8 retriesPerIp)
{
    m_interfaceList = getCurrentlyOnlineInterfacesIPv4();
    if (m_interfaceList.isEmpty())
    {
        // No network interface with connection available.
        return false;
    }
    m_snmpVersion = version;
    m_objectId = objectId;
    m_communityList = communityList;
    m_currentCommunityIndex = 0;
    m_retryCount = m_retriesPerIp = retriesPerIp;
    QNetworkInterface interface = m_interfaceList.takeLast();
    m_firstIp = m_currentIp = getInterfacesLowestIPv4(interface);
    m_lastIp = getInterfacesHighestIPv4(interface);
    SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[0], m_objectId);
    m_datagram = paket.getDatagram();
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    m_sendIntervalTimerId = startTimer(m_sendInterval);

    return true;
}

// Scan a range of ip addresses for SNMP devices.
bool SnmpScanner::scanRange(const long version, const QStringList &communityList, const QString &objectId, const quint8 retriesPerIp, const QHostAddress &start, const QHostAddress &end)
{
    m_interfaceList = QList<QNetworkInterface>();
    m_snmpVersion = version;
    m_objectId = objectId;
    m_communityList = communityList;
    m_currentCommunityIndex = 0;
    m_retryCount = m_retriesPerIp = retriesPerIp;
    m_firstIp = m_currentIp = start.toIPv4Address();
    m_lastIp = end.toIPv4Address();
    SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[0], m_objectId);
    m_datagram = paket.getDatagram();
    writeDatagram(m_datagram, start, m_port);
    m_sendIntervalTimerId = startTimer(m_sendInterval);

    return true;
}

// Send requests to the same IP range as before.
// Reset current ip. Increment retry count and start timer.
void SnmpScanner::doRetry()
{
    if (m_retryCount > 0)
    {
        --m_retryCount;
        m_currentIp = m_firstIp;
        m_sendIntervalTimerId = startTimer(m_sendInterval);
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // All retries done. Scan another community string.
        emit changeSnmpCommunity();
    }
}

// Scan subnet with another community string.
// Take another community string and build a new paket.
// Start from first IP with retries.
void SnmpScanner::scanNextSnmpCommunity()
{
    ++m_currentCommunityIndex;
    if (m_currentCommunityIndex < m_communityList.size())
    {
        m_retryCount = m_retriesPerIp;
        m_currentIp = m_firstIp;
        SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
        m_datagram = paket.getDatagram();
        m_sendIntervalTimerId = startTimer(m_sendInterval);
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // All community string are used. Get next network interface.
        emit scanNextInterface();
    }
}

// Scan subnet of another network interfcae.
void SnmpScanner::scanNextInterface()
{
    if (!m_interfaceList.isEmpty())
    {
        QNetworkInterface interface = m_interfaceList.takeLast();
        m_firstIp = m_currentIp = getInterfacesLowestIPv4(interface);
        m_lastIp = getInterfacesHighestIPv4(interface);
        m_retryCount = m_retriesPerIp;
        m_currentCommunityIndex = 0;
        SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
        m_datagram = paket.getDatagram();
        m_sendIntervalTimerId = startTimer(m_sendInterval);
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // No more interfaces to scan.
        // Wait for answers before leave scanner.
        m_sentAllPackets = true;
        m_sendIntervalTimerId = startTimer(1000);
    }
}

// Got an answer. Read message and store information.
// Store response in a QHash. Use host address as key.
void SnmpScanner::readResponse()
{
    while (hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(pendingDatagramSize());
        QHostAddress host;
        readDatagram(datagram.data(), datagram.size(), &host);
        SnmpPaket paket = SnmpPaket::fromDatagram(datagram);
        m_pResultTable->addDevice(host, paket.community(), paket.pduValue(0));
    }
}


// Find all network interfaces which are currently online.
QList<QNetworkInterface> SnmpScanner::getCurrentlyOnlineInterfacesIPv4() const
{
    QList<QNetworkInterface> onlineInterfaces;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for (int index=0; index<interfaceList.size(); ++index)
    {
        QNetworkInterface interface = interfaceList[index];
        if (!interface.hardwareAddress().isEmpty() && hasInterfaceIPv4Entry(interface))
        {
            onlineInterfaces.append(interface);
        }
    }

    return onlineInterfaces;
}

// Tests if a network interface has a IPv4 address.
bool SnmpScanner::hasInterfaceIPv4Entry(const QNetworkInterface &interface) const
{
    QList<QNetworkAddressEntry> addressEntryList = interface.addressEntries();
    for (int index=0; index<addressEntryList.size(); ++index)
    {
        QNetworkAddressEntry entry = addressEntryList[index];
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
        {
            return true;
        }
    }

    return false;
}

// Get the IPv4 address entry of an interface.
QNetworkAddressEntry SnmpScanner::getInterfacesIPv4Entry(const QNetworkInterface &interface)
{
    QList<QNetworkAddressEntry> addressEntryList = interface.addressEntries();
    for (int index=0; index<addressEntryList.size(); ++index)
    {
        QNetworkAddressEntry entry = addressEntryList[index];
        if (entry.ip().protocol() == QAbstractSocket::IPv4Protocol)
        {
            return entry;
        }
    }

    return QNetworkAddressEntry();
}

// Get the lowest ip address of a network interfaces subnet. But not 0.
quint32 SnmpScanner::getInterfacesLowestIPv4(const QNetworkInterface &interface)
{
    QNetworkAddressEntry addressEntry = getInterfacesIPv4Entry(interface);
    quint32 netmask = addressEntry.netmask().toIPv4Address();
    quint32 ip = addressEntry.ip().toIPv4Address();
    quint32 lowestIp = ip & netmask;
    if (((quint8)lowestIp) == 0)
    {
        lowestIp += 1;
    }

    return lowestIp;
}

// Get the highest ip address of the interfaces subnet. But not Broadcast address.
quint32 SnmpScanner::getInterfacesHighestIPv4(const QNetworkInterface &interface)
{
    QNetworkAddressEntry addressEntry = getInterfacesIPv4Entry(interface);
    quint32 highestIp = addressEntry.broadcast().toIPv4Address();

    return (highestIp - 1);
}

// Timer event to send next paket.
void SnmpScanner::timerEvent(QTimerEvent *event)
{
    Q_UNUSED(event);
    if (m_sentAllPackets) {
        // All packets are sent. Exit scanner with signal 'scanFinished()'.
        killTimer(m_sendIntervalTimerId);
        ScanResult *resultTable = m_pResultTable;
        emit scanFinished(resultTable);
        return;
    }
    // Increment current ip address if posible.
    if (! nextIp())
    {
        // No more Ip in range.
        killTimer(m_sendIntervalTimerId);
        // The whole range are scanned. Do retry on this range.
        emit retry();
        return;
    }
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
}
