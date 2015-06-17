#include "snmpscanner.h"

// Constructor
SnmpScanner::SnmpScanner(QObject *parent)
    : QUdpSocket(parent),
      m_port(161),
      m_pResultTable(new DeviceMap),
      m_sendInterval(2),
      m_sendIntervalTimerId(-1),
      m_sentAllPackets(false)
{
    connect(this, SIGNAL(readyRead()), this, SLOT(readResponse()));
    connect(this, SIGNAL(retry()), this, SLOT(doRetry()));
    connect(this, SIGNAL(changeSnmpCommunity()), SLOT(scanNextSnmpCommunity()));
    connect(this, SIGNAL(changeInterface()), this, SLOT(scanNextInterface()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleError(QAbstractSocket::SocketError)));
}
// Destructor
SnmpScanner::~SnmpScanner()
{
    delete m_pResultTable;
}

// Start SNMP device scan. Send UDP datagram to each ip in available subnets.
bool SnmpScanner::startScan(const long version, const QStringList &communityList, const QString &objectId, const quint8 retriesPerIp)
{
    m_interfaceList = getCurrentlyOnlineInterfacesIPv4();
    if (m_interfaceList.isEmpty())
    {
        // No network interface with connection available.
        m_errorMessage = QString("No online network interface available.");
        return false;
    }
    m_pResultTable->clear();
    m_snmpVersion = version;
    m_objectId = objectId;
    m_communityList = communityList;
    m_currentCommunityIndex = 0;
    m_retryCount = m_retriesPerIp = retriesPerIp;
    QNetworkInterface netAdapter = m_interfaceList.takeLast();
    m_firstIp = m_currentIp = getInterfacesLowestIPv4(netAdapter);
    m_lastIp = getInterfacesHighestIPv4(netAdapter);
    SnmpPacket packet = SnmpPacket::snmpGetRequest(m_snmpVersion, m_communityList[0], m_objectId);
    m_datagram = packet.getDatagram();
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    m_sendIntervalTimerId = startTimer(m_sendInterval);

    return true;
}

// Scan a range of ip addresses for SNMP devices.
bool SnmpScanner::scanRange(const long version, const QStringList &communityList, const QString &objectId, const quint8 retriesPerIp, const QHostAddress &start, const QHostAddress &end)
{
    m_pResultTable->clear();
    m_interfaceList = QList<QNetworkInterface>();
    m_snmpVersion = version;
    m_objectId = objectId;
    m_communityList = communityList;
    m_currentCommunityIndex = 0;
    m_retryCount = m_retriesPerIp = retriesPerIp;
    m_firstIp = m_currentIp = start.toIPv4Address();
    m_lastIp = end.toIPv4Address();
    SnmpPacket packet = SnmpPacket::snmpGetRequest(m_snmpVersion, m_communityList[0], m_objectId);
    m_datagram = packet.getDatagram();
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
        SnmpPacket packet = SnmpPacket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
        m_datagram = packet.getDatagram();
        m_sendIntervalTimerId = startTimer(m_sendInterval);
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // All community strings are used. Get next network interface.
        emit scanNextInterface();
    }
}

// Scan subnet of another network interfcae.
void SnmpScanner::scanNextInterface()
{
    if (!m_interfaceList.isEmpty())
    {
        QNetworkInterface netAdapter = m_interfaceList.takeLast();
        m_firstIp = m_currentIp = getInterfacesLowestIPv4(netAdapter);
        m_lastIp = getInterfacesHighestIPv4(netAdapter);
        m_retryCount = m_retriesPerIp;
        m_currentCommunityIndex = 0;
        SnmpPacket packet = SnmpPacket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
        m_datagram = packet.getDatagram();
        m_sendIntervalTimerId = startTimer(m_sendInterval);
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // No more interfaces to scan.
        // Wait for answers before leave scanner.
        m_sentAllPackets = true;
        m_sendIntervalTimerId = startTimer(2000);
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
        SnmpPacket packet = SnmpPacket::fromDatagram(datagram);
        if (packet.isEmpty())
        {
            continue;
        }
        SnmpDevice device(packet.stringValueAt(0), packet.community(), host);
        m_pResultTable->addDevice(device);
    }
}

// Handle soocket errors.
void SnmpScanner::handleError(QAbstractSocket::SocketError socketError)
{
    Q_UNUSED(socketError)
    // Stop timer (send interval) if is running.
    if (m_sendIntervalTimerId >= 0)
    {
        killTimer(m_sendIntervalTimerId);
        m_sendIntervalTimerId = -1;
    }
    QString errorMessage = QString("UDP socket: ").append(errorString());

    emit reportError(errorMessage);
}


// Find all network interfaces which are currently online.
QList<QNetworkInterface> SnmpScanner::getCurrentlyOnlineInterfacesIPv4() const
{
    QList<QNetworkInterface> onlineInterfaces;
    QList<QNetworkInterface> interfaceList = QNetworkInterface::allInterfaces();
    for (int index=0; index<interfaceList.size(); ++index)
    {
        QNetworkInterface netAdapter = interfaceList[index];
        if (!netAdapter.hardwareAddress().isEmpty() && hasInterfaceIPv4Entry(netAdapter))
        {
            onlineInterfaces.append(netAdapter);
        }
    }

    return onlineInterfaces;
}

// Tests if a network interface has a IPv4 address.
bool SnmpScanner::hasInterfaceIPv4Entry(const QNetworkInterface &netAdapter) const
{
    QList<QNetworkAddressEntry> addressEntryList = netAdapter.addressEntries();
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
QNetworkAddressEntry SnmpScanner::getInterfacesIPv4Entry(const QNetworkInterface &netAdapter)
{
    QList<QNetworkAddressEntry> addressEntryList = netAdapter.addressEntries();
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
quint32 SnmpScanner::getInterfacesLowestIPv4(const QNetworkInterface &netAdapter)
{
    QNetworkAddressEntry addressEntry = getInterfacesIPv4Entry(netAdapter);
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
quint32 SnmpScanner::getInterfacesHighestIPv4(const QNetworkInterface &netAdapter)
{
    QNetworkAddressEntry addressEntry = getInterfacesIPv4Entry(netAdapter);
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
        m_sendIntervalTimerId = -1;
        emit scanFinished(m_pResultTable);
        return;
    }
    // Increment current ip address if posible.
    if (! nextIp())
    {
        // No more Ip in range.
        killTimer(m_sendIntervalTimerId);
        m_sendIntervalTimerId = -1;
        // The whole range are scanned. Do retry on this range.
        emit retry();
        return;
    }
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
}
