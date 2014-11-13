#include "getscheduleresponse.h"
#include "timing/timingfactory.h"
#include "log/logger.h"

LOGGER(GetScheduleResponse);

bool GetScheduleResponse::fillFromVariant(const QVariantMap &variant)
{
    m_tasks.clear();

    foreach (const QVariant &entry, variant.value("objects").toList())
    {
        QVariantMap variantMap = entry.toMap();

        // generate task
        ScheduleDefinition schedule = ScheduleDefinition::fromVariant(variantMap);

        if (m_validator.validate(schedule) == TaskValidator::Valid)
        {
            m_tasks.append(schedule);
        }
        else
        {
            // TODO: Get more information
            LOG_DEBUG(QString("Received invalid task, ignoring."));
        }
    }

    return true;
}

ScheduleDefinitionList GetScheduleResponse::tasks() const
{
    return m_tasks;
}
