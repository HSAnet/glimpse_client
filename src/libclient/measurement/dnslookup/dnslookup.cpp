#include "dnslookup.h"
#include "../../log/logger.h"

#include <QDnsLookup>
#include <QHostAddress>

LOGGER(Dnslookup);

Dnslookup::Dnslookup(QObject *parent)
: Measurement(parent)
, currentStatus(Dnslookup::Unknown)
{
}

Dnslookup::~Dnslookup()
{
}

bool Dnslookup::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<DnslookupDefinition>();
    if ( definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(&dns, SIGNAL(finished()), this, SLOT(handleServers()));

    currentStatus = Dnslookup::Unknown;

    return true;
}

bool Dnslookup::start()
{
    dns.setType(QDnsLookup::ANY);
    dns.setName(definition->host);
    dns.setNameserver(QHostAddress::QHostAddress(definition->dnsServer));
    dns.lookup();
    return true;
}

void Dnslookup::handleServers()
{
    // Check the lookup succeeded.
    if (dns.error() != QDnsLookup::NoError)
    {
        LOG_ERROR(dns.errorString());
        LOG_ERROR("DNS lookup failed");
        emit finished();
        return;
    }

    dnslookupOutput = dns.hostAddressRecords();

    setStatus(Dnslookup::Finished);
    emit finished();
}


Measurement::Status Dnslookup::status() const
{
    return currentStatus;
}

void Dnslookup::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
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
    foreach(const QDnsHostAddressRecord &val, dnslookupOutput)
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
        map.insert("timeToLive", val.timeToLive());
        map.insert("value", val.value().toString());
        map.insert("type", type);

        res << map;
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void Dnslookup::started()
{
    dnslookupOutput.clear();

    setStatus(Dnslookup::Running);
}

void Dnslookup::finished()
{
    setStatus(Dnslookup::Finished);
    emit Measurement::finished();
}
