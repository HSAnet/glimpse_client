#include "getinstructionresponse.h"
#include "log/logger.h"

LOGGER(GetInstructionResponse);

QList<int> GetInstructionResponse::taskIds() const
{
    return m_taskIds;
}

QList<int> GetInstructionResponse::scheduleIds() const
{
    return m_scheduleIds;
}

bool GetInstructionResponse::fillFromVariant(const QVariantMap &variant)
{
    m_taskIds.clear();

    foreach (const QVariant &entry, variant.value("tasks").toList())
    {
        m_taskIds.append(entry.toInt());
    }

    m_scheduleIds.clear();

    foreach (const QVariant &entry, variant.value("schedules").toList())
    {
      m_scheduleIds.append(entry.toInt());
    }

    LOG_DEBUG(QString("Received %1 tasks").arg(m_taskIds.size()));
    LOG_DEBUG(QString("Received %1 schedules").arg(m_scheduleIds.size()));

    return true;
}
