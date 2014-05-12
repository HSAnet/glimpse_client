#include "reverse_dnslookup.h"
#include "../../log/logger.h"

#include <QHostInfo>

LOGGER(ReverseDnslookup);

ReverseDnslookup::ReverseDnslookup(QObject *parent)
: Measurement(parent)
, currentStatus(ReverseDnslookup::Unknown)
{
}

ReverseDnslookup::~ReverseDnslookup()
{
}

bool ReverseDnslookup::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<ReverseDnslookupDefinition>();
    if ( definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(start(), SIGNAL(finished()), this, SLOT(handleServers()));

//    QHostInfo::lookupHost("8.8.8.8", this, SLOT(printResults(QHostInfo)));
    currentStatus = ReverseDnslookup::Unknown;

    return true;
}

bool ReverseDnslookup::start()
{
    lookupHost.lookupHost(definition->ip);
//    QHostInfo::lookupHost("4.2.2.1", this, SLOT(handleServers(QHostInfo)));
    emit finished();
    return true;
}

void ReverseDnslookup::handleServers()
{
    // Check the lookup succeeded.
    if (lookupHost.error() != QHostInfo::NoError)
    {
        LOG_ERROR(lookupHost.errorString());
        LOG_ERROR("IP lookup failed");
        emit finished();
        return;
    }

    reverse_dnslookupOutput = info.hostName();

    setStatus(ReverseDnslookup::Finished);
    emit finished();
}


Measurement::Status ReverseDnslookup::status() const
{
    return currentStatus;
}

void ReverseDnslookup::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool ReverseDnslookup::stop()
{
    return true;
}

ResultPtr ReverseDnslookup::result() const
{
    QVariantList res;
//    foreach(const QDnsHostAddressRecord &val, dnslookupOutput)
//    {
//        QString type;
//        switch(val.value().protocol())
//        {
//        case QAbstractSocket::IPv4Protocol:
//            type = "A";
//            break;
//        case QAbstractSocket::IPv6Protocol:
//            type = "AAAA";
//            break;
//        default:
//            break;
//        }

//        QVariantMap map;
//        map.insert("name", val.name());
//        map.insert("timeToLive", val.timeToLive());
//        map.insert("value", val.value().toString());
//        map.insert("type", type);

//        res << map;
//    }

    QVariantMap map;
    map.insert("hostname", reverse_dnslookupOutput);

    res << map;

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void ReverseDnslookup::started()
{
    dnslookupOutput.clear();

    setStatus(ReverseDnslookup::Running);
}

void ReverseDnslookup::finished()
{
    setStatus(ReverseDnslookup::Finished);
    emit Measurement::finished();
}
