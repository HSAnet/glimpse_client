#include "reverseDnslookup.h"
#include "../../log/logger.h"
#include "../../types.h"
#include "../../client.h"
#include "../../trafficbudgetmanager.h"

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
        setErrorString("Definition is empty");
        return false;
    }

    m_currentStatus = ReverseDnslookup::Unknown;

    /*
     * worst case: 512 bytes
     * best case: 80 bytes
     */
    if (!Client::instance()->trafficBudgetManager()->addUsedTraffic(592))
    {
        setErrorString("not enough traffic available");
        return false;
    }

    return true;
}

bool ReverseDnslookup::start()
{
    m_lookupId = QHostInfo::lookupHost(m_definition->ip, this, SLOT(handleServers(QHostInfo)));
    return true;
}

void ReverseDnslookup::handleServers(QHostInfo info)
{
    // Check the lookup succeeded.
    if (info.error() != QHostInfo::NoError)
    {
        emit error(QString("IP lookup failed: %1").arg(info.errorString()));
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

    foreach (const QHostAddress &hostaddress, m_reverseDnslookupAddresses)
    {
        QVariantList tmp;
        tmp.append(m_reverseDnslookupOutput);
        tmp.append(hostaddress.toString());
        res.append(QVariant(tmp));
    }

    return Result(res);
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
