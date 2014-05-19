#include "dnslookup.h"
#include "../../log/logger.h"

#include <QDnsLookup>
#include <QHostAddress>

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
    if ( m_definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(&m_dns, SIGNAL(finished()), this, SLOT(handleServers()));

    m_currentStatus = Dnslookup::Unknown;

    return true;
}

bool Dnslookup::start()
{
    m_dns.setType(QDnsLookup::ANY);
    m_dns.setName(m_definition->host);
    m_dns.setNameserver(QHostAddress::QHostAddress(m_definition->dnsServer));
    m_dns.lookup();
    return true;
}

void Dnslookup::handleServers()
{
    // Check the lookup succeeded.
    if (m_dns.error() != QDnsLookup::NoError)
    {
        LOG_ERROR(m_dns.errorString());
        LOG_ERROR("DNS lookup failed");
        emit finished();
        return;
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
    return true;
}

ResultPtr Dnslookup::result() const
{
    QVariantList res;
    foreach(const QDnsHostAddressRecord &val, m_dnslookupOutput)
    {
        QString type;
        switch(val.value().protocol())
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
        map.insert("time_to_live", val.timeToLive());
        map.insert("value", val.value().toString());
        map.insert("type", type);

        res << map;
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
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
