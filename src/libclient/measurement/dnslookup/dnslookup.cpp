#include "dnslookup.h"
#include "../../log/logger.h"

#include <QDnsLookup>

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
    dns.setType(QDnsLookup::SRV);
    dns.setName(definition->host);
//    dns.setName("_xmpp-client._tcp.gmail.com");
    dns.lookup();
    return true;
}

void Dnslookup::handleServers()
{
    // Check the lookup succeeded.
    if (dns.error() != QDnsLookup::NoError) {
        LOG_ERROR(dns.errorString());
        LOG_ERROR("DNS lookup failed");
        return;
    }

    dnslookupOutput = dns.serviceRecords();

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
    foreach(const QDnsServiceRecord &val, dnslookupOutput)
    {

      QVariantMap map;
      map.insert("name", val.name());
      map.insert("port", val.port());
      map.insert("priority", val.priority());
      map.insert("target", val.target());
      map.insert("timeToLive", val.timeToLive());
      map.insert("weight", val.weight());
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
//    Q_UNUSED(exitCode);
//    Q_UNUSED(exitStatus);

    setStatus(Dnslookup::Finished);
    emit Measurement::finished();
}
