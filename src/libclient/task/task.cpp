#include "task.h"

class Task::Private : public QSharedData
{
public:
    TaskId id;
    QString method;
    QVariant measurementDefinition;
};

Task::Task()
: d(new Private)
{
}

Task::Task(const Task &other)
: d(other.d)
{
}

Task::Task(const TaskId &id, const QString &method, const QVariant &measurementDefinition)
: d(new Private)
{
    d->id = id;
    d->method = method;
    d->measurementDefinition = measurementDefinition;
}

Task::~Task()
{
}

Task &Task::operator=(const Task &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
}

bool Task::isNull() const
{
    return !d->id.isValid();
}

void Task::setId(const TaskId &id)
{
    d->id = id;
}

TaskId Task::id() const
{
    return d->id;
}

void Task::setMethod(const QString &method)
{
    d->method = method;
}

QString Task::method() const
{
    return d->method;
}

void Task::setMeasurementDefinition(const QVariant &measurementDefinition)
{
    d->measurementDefinition = measurementDefinition;
}

QVariant Task::measurementDefinition() const
{
    return d->measurementDefinition;
}

Task Task::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Task(TaskId(map.value("id").toInt()),
                map.value("method").toString(),
                map.value("options"));
}

QVariant Task::toVariant() const
{
    QVariantMap map;

    map.insert("id", d->id.toInt());
    map.insert("method", d->method);
    map.insert("options", d->measurementDefinition);

    return map;
}
