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

    currentStatus = ReverseDnslookup::Unknown;

    return true;
}

bool ReverseDnslookup::start()
{
    QHostInfo::lookupHost(definition->ip, this, SLOT(handleServers(QHostInfo)));
    return true;
}

void ReverseDnslookup::handleServers(QHostInfo info)
{
    // Check the lookup succeeded.
    if (info.error() != QHostInfo::NoError)
    {
        LOG_ERROR(info.errorString());
        LOG_ERROR("IP lookup failed");
        emit finished();
        return;
    }

    reverse_dnslookupOutput = info.hostName();

    if (reverse_dnslookupOutput == definition->ip) {
            LOG_ERROR("HOST not found");
            emit finished();
            return;
    }

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
    QVariantMap map;
    map.insert("hostname", reverse_dnslookupOutput);

    res << map;

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void ReverseDnslookup::started()
{
    reverse_dnslookupOutput.clear();

    setStatus(ReverseDnslookup::Running);
}

void ReverseDnslookup::finished()
{
    setStatus(ReverseDnslookup::Finished);
    emit Measurement::finished();
}
