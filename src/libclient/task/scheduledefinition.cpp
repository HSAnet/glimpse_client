#include "scheduledefinition.h"
#include "../types.h"
#include "../timing/timingfactory.h"
#include "task.h"

class TaskData : public QSharedData
{
public:
    ScheduleId id;
    TimingPtr timing;
    Precondition precondition;
    Task task;
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
    d->timing = timing;
    d->precondition = precondition;
    d->task = Task(taskId, name, measurementDefinition);
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

QVariant ScheduleDefinition::task() const
{
    return d->task.toVariant();
}

QVariant ScheduleDefinition::toVariant() const
{
    QVariantMap map;

    map.insert("id", d->id.toInt());
    map.insert("timing", d->timing->toVariant());
    map.insert("precondition", d->precondition.toVariant());
    map.insert("task", d->task.toVariant());

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
    return d->task.id();
}

void ScheduleDefinition::setName(const QString &name)
{
    d->task.setMethod(name);
}

QString ScheduleDefinition::name() const
{
    return d->task.method();
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
    d->task.setMeasurementDefinition(measurementDefinition);
}

QVariant ScheduleDefinition::measurementDefinition() const
{
    return d->task.measurementDefinition();
}

void ScheduleDefinition::setPrecondition(const Precondition &precondition)
{
    d->precondition = precondition;
}

Precondition ScheduleDefinition::precondition() const
{
    return d->precondition;
}
