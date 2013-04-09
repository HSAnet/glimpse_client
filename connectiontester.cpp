#include "connectiontester.h"

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QHostAddress>

#include <QNetworkInterface>
#include <QCoreApplication>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QProcess>
#include <QDebug>

#ifdef Q_OS_LINUX
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <arpa/inet.h>
#include <resolv.h>
#elif defined(Q_OS_WIN)
#include <WinSock2.h>
#include <IPHlpApi.h>
#pragma comment(lib, "IPHLPAPI.lib")
#undef interface
#endif // Q_OS_LINUX

class ConnectionTester::Private
{
    Q_DECLARE_TR_FUNCTIONS(ConnectionTester::Private)

public:
    Private(ConnectionTester* q)
    : q(q)
    , result(ConnectionTester::Offline)
    {
    }

    ConnectionTester* q;

    ConnectionTester::ResultType result;

    void checkInterfaces();
    void checkSlow();

    QString findDefaultGateway() const;
    QString findDefaultDNS() const;
    bool canPing(const QString& host) const;

#ifdef Q_OS_MAC
    QString scutilHelper(const QByteArray &command, const QString& searchKey) const;
#endif // Q_OS_MAC
};

void ConnectionTester::Private::checkInterfaces()
{
    QNetworkConfigurationManager mgr;

    if ( !mgr.isOnline() ) {
        qDebug() << "Offline. Checking why ...";

        emit q->message(tr("Offline."), ConnectionTester::Info);

        QList<QNetworkConfiguration> configs;
        foreach(const QNetworkConfiguration config, mgr.allConfigurations(QNetworkConfiguration::Active)) {
            QNetworkSession session(config);
            QNetworkInterface iface = session.interface();
            if ( !iface.isValid() )
                qDebug() << config.name() << "invalid interface";
            else {
                // Ignore loopback devices
                if ( !iface.flags().testFlag(QNetworkInterface::IsLoopBack) )
                    configs.append(config);
            }
        }

        if ( configs.isEmpty() )
            emit q->message(tr("There are no active network devices. Please check your cables!"), ConnectionTester::Info);
        else {
            // There is some interface, now we have to check routings and dns
            // TODO: What now?
        }
    } else {
        //emit q->message(tr("Some interface is online. Checking ..."), ConnectionTester::Info);

        QString gw = findDefaultGateway();
        if ( gw.isEmpty() || gw == "0.0.0.0" )
            q->message(tr("You don't have a default gateway defined. Please check your DHCP/network settings!"), ConnectionTester::Info);
        else {
            if (!canPing(gw))
                q->message(tr("You can't access your default gateway!"), ConnectionTester::Info);
            else if (!canPing("8.8.8.8"))
                q->message(tr("Your default gateway does not forward your connections!"), ConnectionTester::Info);
            else if (!canPing("google.com"))
                q->message(tr("Your DNS resolution seems broken. Internet access works without DNS."), ConnectionTester::Info);
            else
                q->message(tr("Your connection seems to be fine"), ConnectionTester::Info);
        }

        QString dns = findDefaultDNS();
        if ( dns.isEmpty() )
            q->message(tr("You don't have a default dns server defined. Please check your DHCP/network settings!"), ConnectionTester::Info);
    }

    /*
    foreach(const QNetworkConfiguration& config, mgr.allConfigurations()) {
        // Ignore unknown and bluetooth devices
        if (config.bearerType() == QNetworkConfiguration::BearerUnknown ||
            config.bearerType() == QNetworkConfiguration::BearerBluetooth )
        {
            continue;
        }

        qDebug() << config.bearerTypeName() << config.name() << config.state();
    }*/
}

void ConnectionTester::Private::checkSlow()
{
    QNetworkConfigurationManager mgr;

    QNetworkConfiguration config = mgr.defaultConfiguration();
}

QString ConnectionTester::Private::findDefaultGateway() const
{
    // Inspiration: https://github.com/xbmc/xbmc/blob/8edff7ead55f1a31e55425d47885dc96d3d55105/xbmc/network/linux/NetworkLinux.cpp#L165
    QString gw;

#ifdef Q_OS_LINUX
    QFile file("/proc/net/route");
    if ( !file.open(QIODevice::ReadOnly) ) {
        qDebug() << "Failed to open kernel route" << file.errorString();
        return QString();
    }

    QTextStream stream(&file);
    QString line = stream.readLine();
    while (!line.isEmpty()) {
        QStringList parts = line.split('\t');
        if ( !parts.isEmpty() ) {
            if ( parts.at(1) == "00000000" ) { // Find the default route
                QString ip = parts.at(2);
                QStringList realIp;

                for(int pos=0; pos < ip.size(); pos += 2) {
                    QString block = ip.mid(pos, 2);

                    realIp.prepend( QString::number(block.toInt(NULL, 16)) );
                }

                return realIp.join(".");
            }
        }

        line = stream.readLine();
    }
#elif defined(Q_OS_MAC)
    gw = scutilHelper("show State:/Network/Global/IPv4", "Router");
#elif defined(Q_OS_WIN)
    IP_ADAPTER_INFO* pAdapterInfo = new IP_ADAPTER_INFO;

    ULONG outbufferLength = sizeof(IP_ADAPTER_INFO);
    if ( ::GetAdaptersInfo(pAdapterInfo, &outbufferLength) == ERROR_BUFFER_OVERFLOW ) {
        delete pAdapterInfo;
        pAdapterInfo = (IP_ADAPTER_INFO*)malloc(outbufferLength);
    }

    if ( ::GetAdaptersInfo(pAdapterInfo, &outbufferLength) == NO_ERROR ) {
        IP_ADAPTER_INFO* pAdapter = pAdapterInfo;
        while (pAdapter) {
            QString temp = QString::fromLatin1(pAdapter->GatewayList.IpAddress.String);
            if ( temp != "0.0.0.0" )
                gw = temp;

            pAdapter = pAdapter->Next;
        }
    }

    delete pAdapterInfo;
#else
#error Platform gateway code missing!
#endif

    return gw;
}

QString ConnectionTester::Private::findDefaultDNS() const
{
    // Inspiration: https://github.com/xbmc/xbmc/blob/8edff7ead55f1a31e55425d47885dc96d3d55105/xbmc/network/linux/NetworkLinux.cpp#L411
#ifdef Q_OS_LINUX
    res_init();

    for(int i=0; i < _res.nscount; ++i) {
        return QString::fromLatin1(inet_ntoa( ((sockaddr_in*)&_res.nsaddr_list[0])->sin_addr ));
    }
#elif defined(Q_OS_WIN)
    QString dns;
    IP_ADAPTER_ADDRESSES* addresses = NULL;
    ULONG bufferSize = 0;
    if (::GetAdaptersAddresses(AF_INET, 0, NULL, addresses, &bufferSize) == ERROR_BUFFER_OVERFLOW) {
        addresses = (IP_ADAPTER_ADDRESSES*)malloc(bufferSize);
        if (::GetAdaptersAddresses(AF_INET, 0, NULL, addresses, &bufferSize) == NO_ERROR) {
            IP_ADAPTER_ADDRESSES* addr = addresses;
            while (addr) {
                if ( addr->OperStatus == IfOperStatusUp ) {
                    IP_ADAPTER_DNS_SERVER_ADDRESS* p = addr->FirstDnsServerAddress;
                    while (p) {
                        if ( dns.isEmpty() )
                            dns = QHostAddress(p->Address.lpSockaddr).toString();

                        p = p->Next;
                    }
                }

                addr = addr->Next;
            }
        }
    }

    delete addresses;
    return dns;
#elif defined(Q_OS_MAC)
    return scutilHelper("show State:/Network/Global/DNS", "0");
#else
#error Platform dns code missing!
#endif
    return QString();
}

bool ConnectionTester::Private::canPing(const QString &host) const
{
    QProcess process;
    QStringList args;

#ifdef Q_OS_LINUX
    process.setStandardOutputFile("/dev/null");
    process.setStandardErrorFile("/dev/null");

    args << "-c" << "1" // Amount of pings
         << "-n" // Don't resolve hostnames
         << "-W" << "1"; // Timeout
#elif defined(Q_OS_MAC)
    args.clear(); // Apple needs the host name at last
    args << "-c" << "1" // Amount of pings
         << "-t" << "1"; // Timeout
#elif defined(Q_OS_WIN)
    args << "-n" << "1" // Amount of pings
         << "-4" // Stay with IPv4 for now
         << "-w" << "1000"; // Timeout
#else
#error Platform ping code missing!
#endif

    // Hostname is always the last parameter (necessary on osx/android!)
    args << host;

    process.start("ping", args);
    process.waitForFinished();

    return process.exitCode() == 0;
}

#ifdef Q_OS_MAC
QString ConnectionTester::Private::scutilHelper(const QByteArray& command, const QString &searchKey) const
{
    QString result;

    QProcess sc;
    QTextStream stream(&sc);
    sc.start("scutil");
    sc.waitForStarted();
    //sc.write("show State:/Network/Global/IPv4\n");
    sc.write(command);
    sc.write("\n");
    sc.waitForReadyRead();

    QString line = stream.readLine();
    while (!line.isEmpty()) {
        QStringList parts = line.split(':');
        if ( parts.size() == 2 ) {
            if ( parts.at(0).trimmed() == searchKey )
                result = parts.at(1).trimmed();
        }

        line = stream.readLine();
    }

    sc.write("exit\n");
    sc.waitForFinished();

    return result;
}
#endif // Q_OS_MAC

ConnectionTester::ConnectionTester(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

ConnectionTester::~ConnectionTester()
{
    delete d;
}

void ConnectionTester::start()
{
    d->checkInterfaces();
}

ConnectionTester::ResultType ConnectionTester::result() const
{
    return d->result;
}
