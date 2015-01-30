#include "taskvalidator.h"
#include "../log/logger.h"
#include "../measurement/measurementfactory.h"

LOGGER(TaskValidator);

class TaskValidator::Private
{
public:
    Private()
    {
        MeasurementFactory factory;
        availableMeasurements = factory.availableMeasurements();
    }

    QStringList availableMeasurements;
};

TaskValidator::TaskValidator()
: d(new Private)
{
}

TaskValidator::~TaskValidator()
{
    delete d;
}

TaskValidator::ValidationResult TaskValidator::validate(const ScheduleDefinition &testDefinition)
{
    // Simple checks
    if (testDefinition.isNull()
        || testDefinition.timing().isNull())
    {
        return Invalid;
    }

    // Check if we have the measurment
    if (d->availableMeasurements.contains(testDefinition.name()) == false)
    {
        LOG_WARNING(QString("Measurement named '%1' does not exist").arg(testDefinition.name()));
        return Invalid;
    }

    // Check the timing
    TimingPtr timing = testDefinition.timing();

    if (!timing->isValid())
    {
        LOG_INFO(QString("Measurement '%1' has no valid timing, ignoring").arg(testDefinition.name()));
        return Invalid;
    }

    if (timing->nextRun().isNull())
    {
        LOG_INFO(QString("Measurement '%1' has no next run, ignoring").arg(testDefinition.name()));
        return Invalid;
    }

    return Valid;
}
