#include "specificationresponse.h"
#include "log/logger.h"

LOGGER(SpecificationResponse);

class SpecificationResponse::Private
{
public:
    QVariantList specifications;
    ScheduleDefinitionList tasks;

    TaskValidator validator;
};

SpecificationResponse::SpecificationResponse(QObject *parent)
: Response(parent)
, d(new Private)
{

}

SpecificationResponse::~SpecificationResponse()
{
    delete d;
}

bool SpecificationResponse::fillFromVariant(const QVariantMap &variant)
{
    d->specifications = variant.value("contents", QVariantList()).toList();

    d->tasks.clear();

    foreach (const QVariant &entry, variant.value("contents", QVariantList()).toList())
    {
        QVariantMap variantMap = entry.toMap();

        // generate task
        ScheduleDefinition schedule = ScheduleDefinition::fromMPlaneVariant(variantMap);

        if (d->validator.validate(schedule) == TaskValidator::Valid)
        {
            d->tasks.append(schedule);
        }
        else
        {
            LOG_DEBUG(QString("Received invalid specification, ignoring."));
        }
    }


    return true;
}

ScheduleDefinitionList SpecificationResponse::tasks()
{
    return d->tasks;
}
