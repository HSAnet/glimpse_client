#include "task.h"
#include "../types.h"
#include "../timing/timingfactory.h"

TestDefinition::TestDefinition()
: d(new TaskData)
{
}

TestDefinition::TestDefinition(const TestDefinition &other)
: d(other.d)
{
}

TestDefinition::TestDefinition(const quint32 &id, const QString &name, const TimingPtr &timing,
                               const QVariant &measurementDefinition)
: d(new TaskData)
{
    d->id = id;
    d->name = name;
    d->timing = timing;
    d->measurementDefinition = measurementDefinition;
}

bool TestDefinition::isNull() const
{
    return (d->id == 0);
}

void TestDefinition::setId(const quint32 &id)
{
    d->id = id;
}

QVariant TestDefinition::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", d->id);
    hash.insert("method", d->name);
    hash.insert("timing", d->timing->toVariant());
    hash.insert("options", d->measurementDefinition);
    return hash;
}

TestDefinition TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinition(hash.value("id").toInt(),
                          hash.value("method").toString(),
                          TimingFactory::timingFromVariant(hash.value("timing")),
                          hash.value("options"));
}

quint32 TestDefinition::id() const
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
