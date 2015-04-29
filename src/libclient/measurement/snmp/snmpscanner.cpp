#include "snmpscanner.h"

// Constructor
SnmpScanner::SnmpScanner(QObject *parent) : QUdpSocket(parent)
{
    connect(this, SIGNAL(bytesWritten(qint64)), this, SLOT(sendPaketToNextIP()));
    connect(this, SIGNAL(retry()), this, SLOT(doRetry()));
}
// Destructor
SnmpScanner::~SnmpScanner()
{

}

// Start SNMP device scan. Send UDP datagram to each ip in available subnets.
bool SnmpScanner::startScan(const QStringList &communityList, const quint8 retriesPerIp)
{
    m_interfaceList = getCurrentlyOnlineInterfacesIPv4();
    if (m_interfaceList.isEmpty())
    {
        return false;
    }
    m_communityList = communityList;
    SnmpPaket paket = SnmpPaket::getRequest(SNMP_VERSION_2c, m_communityList.takeLast(), QString("sysDescr.0"));
    m_datagram = paket.getDatagram();
    m_retryCount = m_retriesPerIp = retriesPerIp;
    QNetworkInterface interface = m_interfaceList.takeLast();
    m_firstIp = m_currentIp = getInterfacesLowestIPv4(interface);
    m_lastIp = getInterfacesHighestIPv4(interface);
    writeDatagram(m_datagram, QHostAddress(m_currentIp), 161);

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
    writeDatagram(m_datagram, QHostAddress(m_currentIp), 161);
}

// Send request to the same IP range as before.
void SnmpScanner::doRetry()
{
    if (m_retryCount > 0) {
        --m_retryCount;
        m_currentIp = m_firstIp;
        writeDatagram(m_datagram, QHostAddress(m_currentIp), 161);
    } else {
        // ToDo:
        // ----------------------------------- next community string -----------------------------------
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
