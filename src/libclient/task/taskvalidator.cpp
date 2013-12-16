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

TaskValidator::ValidationResult TaskValidator::validate(const TestDefinitionPtr &testDefinition)
{
    // Simple checks
    if (testDefinition.isNull()
        || testDefinition->id().isNull()
        || testDefinition->timing().isNull())
    {
        return Invalid;
    }

    // Check if we have the measurment
    if (d->availableMeasurements.contains(testDefinition->name()) == false) {
        LOG_WARNING(QString("Measurement named '%1' does not exist").arg(testDefinition->name()));
        return Invalid;
    }

    // TODO: Signature checks etc.

    return Valid;
}
