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
        TestDefinition test = TestDefinition::fromVariant(variantMap.value("task"));

        // write schedule id into test FIXME this is an evil hack
        test.setId(TaskId(variantMap.value("id").toInt()));

        // get timing and merge into task FIXME this is an evil hack
        TimingPtr timing = TimingFactory::timingFromVariant(variantMap.value("timing"));
        test.setTiming(timing);

        // get precondition and merge into task FIXME this is an evil hack
        Precondition precondition = Precondition::fromVariant(variantMap.value("precondition"));
        test.setPrecondition(precondition);

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
