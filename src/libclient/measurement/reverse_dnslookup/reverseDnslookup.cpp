#include "reverseDnslookup.h"
#include "../../log/logger.h"
#include "../../types.h"

#include <QHostInfo>

LOGGER(ReverseDnslookup);

ReverseDnslookup::ReverseDnslookup(QObject *parent)
: Measurement(parent)
, m_currentStatus(ReverseDnslookup::Unknown)
, m_lookupId(0)
{
    setResultHeader(QStringList() << "hostname" << "address");
}

ReverseDnslookup::~ReverseDnslookup()
{
}

bool ReverseDnslookup::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    m_definition = measurementDefinition.dynamicCast<ReverseDnslookupDefinition>();

    if (m_definition.isNull())
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    m_currentStatus = ReverseDnslookup::Unknown;

    return true;
}

bool ReverseDnslookup::start()
{
    setStartDateTime(QDateTime::currentDateTime());

    m_lookupId = QHostInfo::lookupHost(m_definition->ip, this, SLOT(handleServers(QHostInfo)));
    return true;
}

void ReverseDnslookup::handleServers(QHostInfo info)
{
    // Check the lookup succeeded.
    if (info.error() != QHostInfo::NoError)
    {
        LOG_DEBUG(QString("IP lookup failed: %1").arg(info.errorString()));
        setErrorString(info.errorString());
    }

    m_reverseDnslookupOutput = info.hostName();
    m_reverseDnslookupAddresses = info.addresses();

    setStatus(ReverseDnslookup::Finished);
    emit finished();
}


Measurement::Status ReverseDnslookup::status() const
{
    return m_currentStatus;
}

void ReverseDnslookup::setStatus(Status status)
{
    if (m_currentStatus != status)
    {
        m_currentStatus = status;
        emit statusChanged(status);
    }
}

bool ReverseDnslookup::stop()
{
    QHostInfo::abortHostLookup(m_lookupId);
    return true;
}

Result ReverseDnslookup::result() const
{
    QVariantList res;

    res.append(m_reverseDnslookupOutput);
    res.append(listToVariant(m_reverseDnslookupAddresses));

    return Result(startDateTime(), endDateTime(), res);
}

void ReverseDnslookup::started()
{
    m_reverseDnslookupOutput.clear();

    setStatus(ReverseDnslookup::Running);
}

void ReverseDnslookup::finished()
{
    setStatus(ReverseDnslookup::Finished);
    setEndDateTime(QDateTime::currentDateTime());
    emit Measurement::finished();
}
