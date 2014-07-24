#include "connectiontester.h"
#include "measurement/ping/ping.h"
#include "types.h"

#include <QNetworkConfigurationManager>
#include <QNetworkConfiguration>
#include <QNetworkSession>
#include <QHostAddress>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrentRun>

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

#ifdef Q_OS_ANDROID
#include <sys/system_properties.h>
#endif

class ConnectionTester::Private : public QObject
{
    Q_OBJECT

public:
    Private(ConnectionTester *q)
    : q(q)
    , result(ConnectionTester::Offline)
    {
        connect(&watcher, SIGNAL(started()), q, SIGNAL(runningChanged()));
        connect(&watcher, SIGNAL(finished()), q, SIGNAL(runningChanged()));

        connect(&watcher, SIGNAL(started()), q, SIGNAL(started()));
        connect(&watcher, SIGNAL(finished()), q, SIGNAL(finished()));
    }

    ConnectionTester *q;

    ConnectionTester::ResultType result;
    QFutureWatcher<void> watcher;

    void checkInterfaces();
    void checkSlow();

    QString findDefaultGateway() const;
    QString findDefaultDNS() const;
    bool canPing(const QString &host, int *averagePing = 0) const;

#ifdef Q_OS_MAC
    QString scutilHelper(const QByteArray &command, const QString &searchKey) const;
#endif // Q_OS_MAC

#ifdef Q_OS_ANDROID
    QString propHelper(const QByteArray &property) const;
#endif // Q_OS_ANDROID
};

void ConnectionTester::Private::checkInterfaces()
{
    q->checkOnline();
    q->findDefaultGateway();
    q->findDefaultDNS();
    q->canPingGateway();
    q->canPingGoogleDnsServer();
    q->canPingGoogleDomain();
}

void ConnectionTester::Private::checkSlow()
{
    /*QNetworkConfigurationManager mgr;

    QNetworkConfiguration config = mgr.defaultConfiguration();
    if (!config.isValid()) {
        emit q->message(tr("No default connection found."), ConnectionTester::Info);
        return;
    }*/

    /*qDebug() << config.name() << config.type();

    if (config.bearerType() == QNetworkConfiguration::BearerWLAN) {
        qDebug() << "WLan: " << config.children().size();
    }*/
}

QString ConnectionTester::Private::findDefaultGateway() const
{
    // Inspiration: https://github.com/xbmc/xbmc/blob/8edff7ead55f1a31e55425d47885dc96d3d55105/xbmc/network/linux/NetworkLinux.cpp#L165
    QString gw;

#ifdef Q_OS_LINUX
    QFile file("/proc/net/route");

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug() << "Failed to open kernel route" << file.errorString();
        return QString();
    }

    QTextStream stream(&file);
    QString line = stream.readLine();

    while (!line.isEmpty())
    {
        QStringList parts = line.split('\t');

        if (!parts.isEmpty())
        {
            if (parts.at(1) == "00000000")     // Find the default route
            {
                QString ip = parts.at(2);
                QStringList realIp;

                for (int pos = 0; pos < ip.size(); pos += 2)
                {
                    QString block = ip.mid(pos, 2);

                    realIp.prepend(QString::number(block.toInt(NULL, 16)));
                }

                return realIp.join(".");
            }
        }

        line = stream.readLine();
    }

#elif defined(Q_OS_MAC)
    gw = scutilHelper("show State:/Network/Global/IPv4", "Router");
#elif defined(Q_OS_WIN)
    IP_ADAPTER_INFO *pAdapterInfo = new IP_ADAPTER_INFO;

    ULONG outbufferLength = sizeof(IP_ADAPTER_INFO);

    if (::GetAdaptersInfo(pAdapterInfo, &outbufferLength) == ERROR_BUFFER_OVERFLOW)
    {
        delete pAdapterInfo;
        pAdapterInfo = (IP_ADAPTER_INFO *)malloc(outbufferLength);
    }

    if (::GetAdaptersInfo(pAdapterInfo, &outbufferLength) == NO_ERROR)
    {
        IP_ADAPTER_INFO *pAdapter = pAdapterInfo;

        while (pAdapter)
        {
            QString temp = QString::fromLatin1(pAdapter->GatewayList.IpAddress.String);

            if (temp != "0.0.0.0")
            {
                gw = temp;
            }

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
#if defined(Q_OS_ANDROID)
    return propHelper("net.dns1");
#elif defined(Q_OS_LINUX)
    res_init();

    for (int i = 0; i < _res.nscount; ++i)
    {
        return QString::fromLatin1(inet_ntoa(((sockaddr_in *)&_res.nsaddr_list[0])->sin_addr));
    }

    return QString();
#elif defined(Q_OS_WIN)
    QString dns;
    IP_ADAPTER_ADDRESSES *addresses = NULL;
    ULONG bufferSize = 0;

    if (::GetAdaptersAddresses(AF_INET, 0, NULL, addresses, &bufferSize) == ERROR_BUFFER_OVERFLOW)
    {
        addresses = (IP_ADAPTER_ADDRESSES *)malloc(bufferSize);

        if (::GetAdaptersAddresses(AF_INET, 0, NULL, addresses, &bufferSize) == NO_ERROR)
        {
            IP_ADAPTER_ADDRESSES *addr = addresses;

            while (addr)
            {
                if (addr->OperStatus == IfOperStatusUp)
                {
                    IP_ADAPTER_DNS_SERVER_ADDRESS *p = addr->FirstDnsServerAddress;

                    while (p)
                    {
                        if (dns.isEmpty())
                        {
                            dns = QHostAddress(p->Address.lpSockaddr).toString();
                        }

                        p = p->Next;
                    }
                }

                addr = addr->Next;
            }
        }
    }

    free(addresses);
    return dns;
#elif defined(Q_OS_MAC)
    return scutilHelper("show State:/Network/Global/DNS", "0");
#else
#error Platform dns code missing!
    return QString();
#endif
}

bool ConnectionTester::Private::canPing(const QString &host, int *averagePing) const
{
    // TODO: invoke scheduler or tell scheduler something is going on outside of its control
    PingDefinitionPtr pingDef(new PingDefinition(host, 4, 200, 1000, 64, 0, 0, 0, ping::System));
    Ping ping;
    ping.prepare(NULL, pingDef);
    ping.start();
    ping.waitForFinished();

    int resultAvg = ping.averagePingTime();

    if (averagePing)
    {
        *averagePing = resultAvg;
    }

    if (resultAvg > 0)
    {
        return true;
    }
    else
    {
        return false;
    }
}

#ifdef Q_OS_MAC
QString ConnectionTester::Private::scutilHelper(const QByteArray &command, const QString &searchKey) const
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

    while (!line.isEmpty())
    {
        QStringList parts = line.split(':');

        if (parts.size() == 2)
        {
            if (parts.at(0).trimmed() == searchKey)
            {
                result = parts.at(1).trimmed();
            }
        }

        line = stream.readLine();
    }

    sc.write("exit\n");
    sc.waitForFinished();

    return result;
}
#endif // Q_OS_MAC

#ifdef Q_OS_ANDROID
QString ConnectionTester::Private::propHelper(const QByteArray &property) const
{
    // See http://www.netmite.com/android/mydroid/system/core/toolbox/getprop.c
    QByteArray gw;
    gw.resize(200);

    __system_property_get(property, gw.data());

    return gw;
}
#endif // Q_OS_ANDROID

ConnectionTester::ConnectionTester(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
    qRegisterMetaType<TestType>();
}

ConnectionTester::~ConnectionTester()
{
    d->watcher.waitForFinished();
    delete d;
}

bool ConnectionTester::isRunning() const
{
    return d->watcher.isRunning();
}

void ConnectionTester::start()
{
    QFuture<void> future = QtConcurrent::run(d, &ConnectionTester::Private::checkInterfaces);
    d->watcher.setFuture(future);
}

ConnectionTester::ResultType ConnectionTester::result() const
{
    return d->result;
}

bool ConnectionTester::checkOnline()
{
    emit checkStarted(ActiveInterface);
    QNetworkConfigurationManager mgr;
    bool online = mgr.isOnline();
    emit checkFinished(ActiveInterface, online, QVariant::fromValue(online));
    return online;
}

QString ConnectionTester::findDefaultGateway()
{
    emit checkStarted(DefaultGateway);
    QString gw = d->findDefaultGateway();
    emit checkFinished(DefaultGateway, !gw.isNull() && gw != "0.0.0.0", gw);
    return gw;
}

QString ConnectionTester::findDefaultDNS()
{
    emit checkStarted(DefaultDns);
    QString dns = d->findDefaultDNS();
    emit checkFinished(DefaultDns, !dns.isNull(), dns);
    return dns;
}

bool ConnectionTester::canPingGateway()
{
    return canPing(PingDefaultGateway, d->findDefaultGateway());
}

bool ConnectionTester::canPingGoogleDnsServer()
{
    return canPing(PingGoogleDnsServer, "8.8.8.8");
}

bool ConnectionTester::canPingGoogleDomain()
{
    return canPing(PingGoogleDomain, "google.com");
}

bool ConnectionTester::canPing(ConnectionTester::TestType testType, const QString &host)
{
    int avgPing = 0;
    emit checkStarted(testType);
    bool success = d->canPing(host, &avgPing);
    emit checkFinished(testType, success, QVariant::fromValue(avgPing));
    return success;
}

ConnectionTesterModel::ConnectionTesterModel(QObject *parent)
: QAbstractListModel(parent)
, m_connectionTester(NULL)
{
}

ConnectionTesterModel::~ConnectionTesterModel()
{
}

void ConnectionTesterModel::setConnectionTester(ConnectionTester *connectionTester)
{
    if (m_connectionTester == connectionTester)
    {
        return;
    }

    if (m_connectionTester)
    {
        disconnect(m_connectionTester, SIGNAL(started()), this, SLOT(onStarted()));
        disconnect(m_connectionTester, SIGNAL(finished()), this, SLOT(onFinished()));
        disconnect(m_connectionTester, SIGNAL(checkStarted(ConnectionTester::TestType)), this,
                   SLOT(onCheckStarted(ConnectionTester::TestType)));
        disconnect(m_connectionTester, SIGNAL(checkFinished(ConnectionTester::TestType, bool, QVariant)), this,
                   SLOT(onCheckFinished(ConnectionTester::TestType, bool, QVariant)));
    }

    m_connectionTester = connectionTester;

    if (m_connectionTester)
    {
        connect(m_connectionTester, SIGNAL(started()), this, SLOT(onStarted()));
        connect(m_connectionTester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(m_connectionTester, SIGNAL(checkStarted(ConnectionTester::TestType)), this,
                SLOT(onCheckStarted(ConnectionTester::TestType)));
        connect(m_connectionTester, SIGNAL(checkFinished(ConnectionTester::TestType, bool, QVariant)), this,
                SLOT(onCheckFinished(ConnectionTester::TestType, bool, QVariant)));
    }
}

ConnectionTester *ConnectionTesterModel::connectionTester() const
{
    return m_connectionTester;
}

QVariant ConnectionTesterModel::result() const
{
    // Prepare upnp data
    QVariantMap upnp;
    QListIterator<RowData> iter(m_rows);

    while (iter.hasNext())
    {
        const RowData &row = iter.next();

        QString name = enumToString(ConnectionTester, TestType, row.testType);
        name = name.replace(QRegExp("([A-Z])"), "-\\1").toLower();
        name.remove(0, 1);

        upnp.insert(name, row.result);
    }

    return upnp;
}

QHash<int, QByteArray> ConnectionTesterModel::roleNames() const
{
    QHash<int, QByteArray> roleNames;
    roleNames.insert(TestNameRole, "testName");
    roleNames.insert(TestTypeRole, "testType");
    roleNames.insert(TestFinishedRole, "testFinished");
    roleNames.insert(TestResultRole, "testResult");
    roleNames.insert(TestSuccessRole, "testSuccess");
    return roleNames;
}

int ConnectionTesterModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
    {
        return 0;
    }

    return m_rows.size();
}

QVariant ConnectionTesterModel::data(const QModelIndex &index, int role) const
{
    if (index.row() < 0 || index.row() >= m_rows.size())
    {
        return QVariant();
    }

    const RowData &data = m_rows.at(index.row());

    switch (role)
    {
    case TestNameRole:
    {
        switch (data.testType)
        {
        case ConnectionTester::ActiveInterface:
            return tr("Online state");

        case ConnectionTester::DefaultGateway:
            return tr("Gateway");

        case ConnectionTester::DefaultDns:
            return tr("DNS");

        case ConnectionTester::PingDefaultGateway:
            return tr("Access Gateway");

        case ConnectionTester::PingGoogleDnsServer:
            return tr("Access IP");

        case ConnectionTester::PingGoogleDomain:
            return tr("Access Google");

        default:
            break;
        }
    }

    return enumToString(ConnectionTester, TestType, data.testType);

    case TestTypeRole:
        return (int)data.testType;

    case TestFinishedRole:
        return QVariant::fromValue(data.finished);

    case TestResultRole:
        return data.result;

    case TestSuccessRole:
        return QVariant::fromValue(data.success);

    default:
        break;
    }

    return QVariant();
}

void ConnectionTesterModel::onStarted()
{
    beginResetModel();
    m_rows.clear();
    endResetModel();
}

void ConnectionTesterModel::onCheckStarted(ConnectionTester::TestType testType)
{
    int pos = m_rows.size();
    beginInsertRows(QModelIndex(), pos, pos);
    {
        RowData data;
        data.testType = testType;
        data.success = false;
        data.finished = false;

        m_rows.append(data);
    }
    endInsertRows();
}

void ConnectionTesterModel::onCheckFinished(ConnectionTester::TestType testType, bool success, const QVariant &result)
{
    Q_UNUSED(testType);

    RowData &data = m_rows.last();
    data.success = success;
    data.result = result;
    data.finished = true;

    QModelIndex idx = index(m_rows.size() - 1);
    emit dataChanged(idx, idx);
}

void ConnectionTesterModel::onFinished()
{
}

#include "connectiontester.moc"
