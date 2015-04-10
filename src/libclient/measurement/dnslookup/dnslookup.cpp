#include "dnslookup.h"
#include "../../log/logger.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

#include <QDnsLookup>
#include <QHostAddress>
#include "../../types.h"

LOGGER(Dnslookup);

Dnslookup::Dnslookup(QObject *parent)
: Measurement(parent)
, m_currentStatus(Dnslookup::Unknown)
{
}

Dnslookup::~Dnslookup()
{
}

bool Dnslookup::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    m_definition = measurementDefinition.dynamicCast<DnslookupDefinition>();

    if (m_definition.isNull())
    {
        setErrorString("Definition is empty");
        return false;
    }

    connect(&m_dns, SIGNAL(finished()), this, SLOT(handleServers()));

    m_currentStatus = Dnslookup::Unknown;

    m_dns.setType(QDnsLookup::ANY);
    m_dns.setName(m_definition->host);

#if QT_VERSION >= QT_VERSION_CHECK(5, 3, 0)

    if (!m_definition->dnsServer.isEmpty())
    {
        QHostAddress hostAddress(m_definition->dnsServer);
        m_dns.setNameserver(hostAddress);
    }

#endif

    /*
     * worst case: 512 bytes
     * best case: 74 bytes
     * average: 293 bytes
     */
    if (!Client::instance()->trafficBudgetManager()->addUsedTraffic(586))
    {
        setErrorString("not enough traffic available");
        return false;
    }

    return true;
}

bool Dnslookup::start()
{
    m_dns.lookup();
    return true;
}

void Dnslookup::handleServers()
{
    // Check the lookup succeeded.
    if (m_dns.error() != QDnsLookup::NoError)
    {
        emit error(QString("DNS lookup failed: %1").arg(m_dns.errorString()));
        m_dnsError = enumToString(QDnsLookup, Error, m_dns.error());
    }

    m_dnslookupOutput = m_dns.hostAddressRecords();

    setStatus(Dnslookup::Finished);
    emit finished();
}


Measurement::Status Dnslookup::status() const
{
    return m_currentStatus;
}

void Dnslookup::setStatus(Status status)
{
    if (m_currentStatus != status)
    {
        m_currentStatus = status;
        emit statusChanged(status);
    }
}

bool Dnslookup::stop()
{
    m_dns.abort();
    return true;
}

Result Dnslookup::result() const
{
    QVariantMap res;

    QVariantList records;

    foreach (const QDnsHostAddressRecord &val, m_dnslookupOutput)
    {
        QString type;

        switch (val.value().protocol())
        {
        case QAbstractSocket::IPv4Protocol:
            type = "A";
            break;

        case QAbstractSocket::IPv6Protocol:
            type = "AAAA";
            break;

        default:
            break;
        }

        QVariantMap map;
        map.insert("name", val.name());
        map.insert("ttl", val.timeToLive());
        map.insert("value", val.value().toString());
        map.insert("type", type);

        records << map;
    }

    res.insert("records", records);

    Result result(records);
    result.setErrorString(m_dnsError);
    return result;
}

void Dnslookup::started()
{
    m_dnslookupOutput.clear();

    setStatus(Dnslookup::Running);
}

void Dnslookup::finished()
{
    setStatus(Dnslookup::Finished);
    emit Measurement::finished();
}
