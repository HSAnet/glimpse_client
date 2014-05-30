#include "reverseDnslookup.h"
#include "../../log/logger.h"

#include <QHostInfo>

LOGGER(ReverseDnslookup);

ReverseDnslookup::ReverseDnslookup(QObject *parent)
: Measurement(parent)
, m_currentStatus(ReverseDnslookup::Unknown)
, m_lookupId(0)
{
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
    }

    m_reverseDnslookupOutput = info.hostName();
    m_reverseDnslookupAddresses = info.addresses();
    m_reverseDnslookupError = info.errorString();

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

ResultPtr ReverseDnslookup::result() const
{
    QVariantList res;
    QVariantMap map;
    map.insert("error", m_reverseDnslookupError);
    map.insert("hostname", m_reverseDnslookupOutput);

    if (!m_reverseDnslookupAddresses.isEmpty())
    {
        foreach (const QHostAddress& address, m_reverseDnslookupAddresses)
        {
            map.insert("address", address.toString());
        }
    }

    res << map;

    return ResultPtr(new Result(startDateTime(), QDateTime::currentDateTime(), res, QVariant()));
}

void ReverseDnslookup::started()
{
    m_reverseDnslookupOutput.clear();

    setStatus(ReverseDnslookup::Running);
}

void ReverseDnslookup::finished()
{
    setStatus(ReverseDnslookup::Finished);
    emit Measurement::finished();
}
