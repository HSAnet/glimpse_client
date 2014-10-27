#include "task.h"
#include "../types.h"
#include "../timing/timingfactory.h"

class TaskData : public QSharedData
{
public:
    TaskId id;
    QString name;
    TimingPtr timing;
    QVariant measurementDefinition;
    Precondition precondition;
};

TestDefinition::TestDefinition()
: d(new TaskData)
{
}

TestDefinition::TestDefinition(const TestDefinition &other)
: d(other.d)
{
}

TestDefinition::TestDefinition(const TaskId &id, const QString &name, const TimingPtr &timing,
                               const QVariant &measurementDefinition, const Precondition &precondition)
: d(new TaskData)
{
    d->id = id;
    d->name = name;
    d->timing = timing;
    d->measurementDefinition = measurementDefinition;
    d->precondition = precondition;
}

TestDefinition::~TestDefinition()
{
}

TestDefinition &TestDefinition::operator=(const TestDefinition &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
}

bool TestDefinition::isNull() const
{
    return d->id.isValid();
}

void TestDefinition::setId(const TaskId &id)
{
    d->id = id;
}

QVariant TestDefinition::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", d->id.toInt());
    hash.insert("method", d->name);
    hash.insert("timing", d->timing->toVariant());
    hash.insert("options", d->measurementDefinition);
    hash.insert("precondition", d->precondition.toVariant());
    return hash;
}

TestDefinition TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinition(TaskId(hash.value("id").toInt()),
                          hash.value("method").toString(),
                          TimingFactory::timingFromVariant(hash.value("timing")),
                          hash.value("options"),
                          Precondition::fromVariant(hash.value("precondition")));
}

TaskId TestDefinition::id() const
{
    return d->id;
}

void TestDefinition::setName(const QString &name)
{
    d->name = name;
}

QString TestDefinition::name() const
{
    return d->name;
}

void TestDefinition::setTiming(const TimingPtr &timing)
{
    d->timing = timing;
}

TimingPtr TestDefinition::timing() const
{
    return d->timing;
}

void TestDefinition::setMeasurementDefinition(const QVariant &measurementDefinition)
{
    d->measurementDefinition = measurementDefinition;
}

QVariant TestDefinition::measurementDefinition() const
{
    return d->measurementDefinition;
}

void TestDefinition::setPrecondition(const Precondition &precondition)
{
    d->precondition = precondition;
}

Precondition TestDefinition::precondition() const
{
    return d->precondition;
}
