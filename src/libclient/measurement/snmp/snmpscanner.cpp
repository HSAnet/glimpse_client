#include "snmpscanner.h"

// Constructor
SnmpScanner::SnmpScanner(QObject *parent)
    : QUdpSocket(parent),
      m_port(161)
{
    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(sendPaketToNextIP()));
    connect(this, SIGNAL(retry()), this, SLOT(doRetry()));
    connect(this, SIGNAL(changeSnmpCommunity()), SLOT(scanNextSnmpCommunity()));
    connect(this, SIGNAL(readyRead()), this, SLOT(readResponse()));
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
        return false;
    }
    m_snmpVersion = version;
    m_objectId = objectId;
    m_communityList = communityList;
    m_currentCommunityIndex = 0;
    SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
    m_datagram = paket.getDatagram();
    m_retryCount = m_retriesPerIp = retriesPerIp;
    QNetworkInterface interface = m_interfaceList.takeLast();
    m_firstIp = m_currentIp = getInterfacesLowestIPv4(interface);
    m_lastIp = getInterfacesHighestIPv4(interface);
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);

    return true;
}

// Get next ip address and send SNMP message to it.
void SnmpScanner::sendPaketToNextIP()
{
    if (nextIp())
    {
        // The whole range are scanned. Do retry on this range.
        emit retry();
        return;
    }
    writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
}

// Send request to the same IP range as before.
void SnmpScanner::doRetry()
{
    if (m_retryCount > 0)
    {
        --m_retryCount;
        m_currentIp = m_firstIp;
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // All retries done. Scan another community string.
        emit changeSnmpCommunity();
    }
}

// Scan subnet with another community string.
void SnmpScanner::scanNextSnmpCommunity()
{
    ++m_currentCommunityIndex;
    if (m_currentCommunityIndex < m_communityList.size())
    {
        m_retryCount = m_retriesPerIp;
        m_currentIp = m_firstIp;
        SnmpPaket paket = SnmpPaket::snmpGetRequest(m_snmpVersion, m_communityList[m_currentCommunityIndex], m_objectId);
        m_datagram = paket.getDatagram();
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // ToDo:
        // next interface.
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
        writeDatagram(m_datagram, QHostAddress(m_currentIp), m_port);
    }
    else
    {
        // ToDo:
        // Scan is over.
        // The scan result should be analysed.
    }
}

// Got an answer. Read message and store information.
// Store response in a QHash. Use host address as key.
// Value of QHash: Community string and PDU value as QPair
void SnmpScanner::readResponse()
{
    while (hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(pendingDatagramSize());
        QHostAddress host;
        readDatagram(datagram.data(), datagram.size(), &host);
        SnmpPaket paket = SnmpPaket::fromDatagram(datagram);
        QPair<QString, QString> response;
        response.first = paket.community();
        response.second = paket.pduValue(0);
        m_resultMap.insert(host, response);
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
