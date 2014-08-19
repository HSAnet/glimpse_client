#include "getscheduleresponse.h"
#include "timing/timingfactory.h"
#include "log/logger.h"

LOGGER(GetScheduleResponse);

bool GetScheduleResponse::fillFromVariant(const QVariantMap &variant)
{

    foreach (const QVariant &entry, variant.value("objects").toList())
    {
        QVariantMap variantMap = entry.toMap();

        // generate task
        TestDefinition test = TestDefinition::fromVariant(variantMap.value("task"));

        // get timing and merge into task FIXME this is an evil hack
        TimingPtr timing = TimingFactory::timingFromVariant(variantMap.value("timing"));
        test.setTiming(timing);

        if (m_validator.validate(test) == TaskValidator::Valid)
        {
            m_tasks.append(test);
        }
        else
        {
            // TODO: Get more information
            LOG_DEBUG(QString("Received invalid task, ignoring."));
        }
    }

    return true;
}

TestDefinitionList GetScheduleResponse::tasks() const
{
    return m_tasks;
}
