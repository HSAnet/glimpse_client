#include "scheduledefinition.h"
#include "../types.h"
#include "../timing/timingfactory.h"

class TaskData : public QSharedData
{
public:
    ScheduleId id;
    TaskId taskId;
    QString name;
    TimingPtr timing;
    QVariant measurementDefinition;
    Precondition precondition;
};

ScheduleDefinition::ScheduleDefinition()
: d(new TaskData)
{
}

ScheduleDefinition::ScheduleDefinition(const ScheduleDefinition &other)
: d(other.d)
{
}

ScheduleDefinition::ScheduleDefinition(const ScheduleId &id, const TaskId& taskId, const QString &name, const TimingPtr &timing,
                               const QVariant &measurementDefinition, const Precondition &precondition)
: d(new TaskData)
{
    d->id = id;
    d->taskId = taskId;
    d->name = name;
    d->timing = timing;
    d->measurementDefinition = measurementDefinition;
    d->precondition = precondition;

    // TODO: save scheduleDefinition as JSON
}

ScheduleDefinition::~ScheduleDefinition()
{
}

ScheduleDefinition &ScheduleDefinition::operator=(const ScheduleDefinition &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
}

bool ScheduleDefinition::isNull() const
{
    return !d->id.isValid();
}

void ScheduleDefinition::setId(const ScheduleId &id)
{
    d->id = id;
}

QVariant ScheduleDefinition::toVariant() const
{
    QVariantMap map;
    QVariantMap task;

    task.insert("id", d->taskId.toInt());
    task.insert("method", d->name);
    task.insert("options", d->measurementDefinition);

    map.insert("id", d->id.toInt());
    map.insert("timing", d->timing->toVariant());
    map.insert("precondition", d->precondition.toVariant());
    map.insert("task", task);

    return map;
}

ScheduleDefinition ScheduleDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    QVariantMap task = map.value("task").toMap();

    return ScheduleDefinition(ScheduleId(map.value("id").toInt()),
                              TaskId(task.value("id").toInt()),
                              task.value("method").toString(),
                              TimingFactory::timingFromVariant(map.value("timing")),
                              task.value("options"),
                              Precondition::fromVariant(map.value("precondition")));
}

ScheduleId ScheduleDefinition::id() const
{
    return d->id;
}

TaskId ScheduleDefinition::taskId() const
{
    return d->taskId;
}

void ScheduleDefinition::setName(const QString &name)
{
    d->name = name;
}

QString ScheduleDefinition::name() const
{
    return d->name;
}

void ScheduleDefinition::setTiming(const TimingPtr &timing)
{
    d->timing = timing;
}

TimingPtr ScheduleDefinition::timing() const
{
    return d->timing;
}

void ScheduleDefinition::setMeasurementDefinition(const QVariant &measurementDefinition)
{
    d->measurementDefinition = measurementDefinition;
}

QVariant ScheduleDefinition::measurementDefinition() const
{
    return d->measurementDefinition;
}

void ScheduleDefinition::setPrecondition(const Precondition &precondition)
{
    d->precondition = precondition;
}

Precondition ScheduleDefinition::precondition() const
{
    return d->precondition;
}
