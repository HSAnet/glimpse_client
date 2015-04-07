#include "networkhelper.h"

#include <QNetworkInterface>
#include <QHostInfo>
#include <QStringList>
#include <QProcess>
#include <QFile>

bool RemoteHost::isValid() const
{
    return !host.isEmpty();
}

/*
 * This code is based on code I found here:
 * http://qt-project.org/forums/viewreply/100389/
 */

/**
 Link Local address should be in the range
 169.254.1.0 to 169.254.254.255
 Refer :http://en.wikipedia.org/wiki/Private_network
 */
bool NetworkHelper::isLinkLocalAddress(const QHostAddress &addr)
{
    quint32 hostIpv4Addr = addr.toIPv4Address();
    quint32 rangeMin = QHostAddress("169.254.1.0").toIPv4Address();
    quint32 rangeMax = QHostAddress("169.254.254.255").toIPv4Address();
    return (hostIpv4Addr >= rangeMin && hostIpv4Addr <= rangeMax);
}

/**
 Refer: http://en.wikipedia.org/wiki/Private_network
 Private Ipv4 address ranges:
  1. 10.0.0.0 - 10.255.255.255
  2. 172.16.0.0 - 172.31.255.255
  3. 192.168.0.0 - 192.168.255.255
 */
bool NetworkHelper::isLocalIpAddress(const QHostAddress &addr)
{
    quint32 hostIpv4Addr = addr.toIPv4Address();
    quint32 range1Min = QHostAddress("10.0.0.0").toIPv4Address();
    quint32 range1Max = QHostAddress("10.255.255.255").toIPv4Address();

    quint32 range3Min = QHostAddress("172.16.0.0").toIPv4Address();
    quint32 range3Max = QHostAddress("172.31.255.255").toIPv4Address();

    quint32 range2Min = QHostAddress("192.168.0.0").toIPv4Address();
    quint32 range2Max = QHostAddress("192.168.255.255").toIPv4Address();

    return ((hostIpv4Addr >= range1Min && hostIpv4Addr <= range1Max)
            || (hostIpv4Addr >= range2Min && hostIpv4Addr <= range2Max)
            || (hostIpv4Addr >= range3Min && hostIpv4Addr <= range3Max));
}

QHostAddress NetworkHelper::localIpAddress()
{
    QHostInfo hostInfo = QHostInfo::fromName(QHostInfo::localHostName());
    QList<QHostAddress> hostNameLookupAddressList = hostInfo.addresses();
    QList<QHostAddress> interfaceAddressList = QNetworkInterface::allAddresses();

    //qDebug()<<__FUNCTION__<<"hostName lookup addresses:"<<hostNameLookupAddressList;
    //qDebug()<<__FUNCTION__<<"interface addresses:"<<interfaceAddressList;

    QHostAddress hostIp;
#if defined(Q_OS_WIN) || defined(Q_OS_MAC)

    foreach (QHostAddress addr, hostNameLookupAddressList)
    {
        if (addr.protocol() == QAbstractSocket::IPv4Protocol && interfaceAddressList.contains(addr))
        {
            if (isLocalIpAddress(addr))
            {
                //qDebug() << __FUNCTION__ << addr << " is local ip";
                hostIp = addr;
                break;
            }
            else if (isLinkLocalAddress(addr))
            {
                //qDebug() << __FUNCTION__ << addr << " is Link Local Address";
                hostIp = addr;
            }
            else
            {
                //qDebug() << __FUNCTION__ << addr << "is some different address";
            }
        }
    }

#else

    foreach (const QHostAddress &addr, interfaceAddressList)
    {
        if (addr.protocol() != QAbstractSocket::IPv4Protocol)
        {
            interfaceAddressList.removeAll(addr);
        }

        if (addr.toString().startsWith("127."))
        {
            interfaceAddressList.removeAll(addr);
        }
    }

    if (!interfaceAddressList.empty())
    {
        hostIp = interfaceAddressList.first();
    }

    //qDebug() << "Hope" << hostIp << "is a local ip";

#endif

    return hostIp;
}

QHostAddress NetworkHelper::gatewayIpAddress()
{
    QProcess process;
    QByteArray output;
    QHostAddress ret;
#if defined(Q_OS_LINUX)
    // default via 192.168.0.1 dev wlp3s0
    QRegExp re("^default via (\\d+\\.\\d+\\.\\d+\\.\\d+)");

    process.start("ip", QStringList("route"));

    while (process.waitForReadyRead())
    {
        output += process.readAll();
    }

    QStringList entries = QString::fromLocal8Bit(output).split('\n');
    entries.removeLast();

    foreach (const QString &s, entries)
    {
        if (re.indexIn(s) > -1)
        {
            ret = QHostAddress(re.cap(1));
            break;
        }
    }
#else
    // 0.0.0.0  0.0.0.0  192.168.0.1  192.168.0.11  10
    QRegExp re("(\\d+\\.\\d+\\.\\d+\\.\\d+).*(\\d+\\.\\d+\\.\\d+\\.\\d+).*\\d+\\.\\d+\\.\\d+\\.\\d+");

#if defined(Q_OS_MAC)
    process.start("netstat", QStringList("-nr"));
#elif defined(Q_OS_WIN)
    process.start("route", QStringList("PRINT"));
#endif

    while (process.waitForReadyRead())
    {
        output += process.readAll();
    }

    QStringList entries = QString::fromLocal8Bit(output).split('\n');
    entries.removeLast();

    foreach (const QString &s, entries)
    {
        if (re.indexIn(s) == -1)
        {
            continue;
        }

        if (re.cap(1) == "0.0.0.0")
        {
            ret = QHostAddress(re.cap(2));
            break;
        }
    }
#endif

    return ret;
}

QList<QHostAddress> NetworkHelper::dnsServerIpAddresses()
{
    QList<QHostAddress> ret;
#if defined(Q_OS_LINUX)
    QRegExp re("nameserver(\\t| )+([^ ]+)");
    QFile file("/etc/resolv.conf");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return ret;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (re.indexIn(line) == -1)
        {
            continue;
        }

        QHostAddress addr(re.cap(2));
        ret.append(addr);
    }
#elif defined(Q_OS_OSX)
    // TODO: use networksetup command as /etc/resolv.conf is not used
#elif defined(Q_OS_WIN)
    QRegExp re("DNS Servers");
    QRegExp re_ipv4("(\\d+\\.\\d+\\.\\d+\\.\\d+)");
    QRegExp re_ipv6("(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|"
                    "([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|"
                    "([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|"
                    "([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|"
                    ":((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|"
                    "::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|"
                    "(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|"
                    "1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))");
    QProcess process;
    QByteArray output;
    bool dnsSection = false;

    process.start("ipconfig", QStringList("/all"));

    while (process.waitForReadyRead())
    {
        output += process.readAll();
    }

    QStringList entries = QString::fromLocal8Bit(output).split('\n');
    entries.removeLast();

    foreach (const QString &s, entries)
    {
        if (re.indexIn(s) == -1 && !dnsSection)
        {
            continue;
        }

        dnsSection = true;
        QHostAddress addr;

        if (re_ipv4.indexIn(s) != -1)
        {
            addr = re_ipv4.cap(1);
        }
        else if (re_ipv6.indexIn(s) != -1)
        {
            addr = re_ipv6.cap(1);
        }
        else
        {
            // end of DNS server list
            break;
        }

        ret.append(addr);

        if (ret.size() == 4)
        {
            // Windows allows 2 DNS servers for each IP version, i.e. 4 servers in total
            break;
        }
    }
#endif
    return ret;
}

RemoteHost NetworkHelper::remoteHost(const QString &hostname)
{
    RemoteHost host;

    if (hostname.contains(':'))
    {
        QStringList parts = hostname.split(':');
        host.host = parts.at(0);
        host.port = parts.at(1).toInt();
    }
    else
    {
        host.host = hostname;
        host.port = 5105;
    }

    return host;
}
