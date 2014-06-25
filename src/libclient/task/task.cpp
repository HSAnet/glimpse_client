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

TestDefinition::TestDefinition(const QUuid &id, const QString &name, const TimingPtr &timing,
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
    return d->ref == 1;
}

void TestDefinition::setId(const QUuid &id)
{
    d->id = id;
}

QVariant TestDefinition::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", uuidToString(d->id));
    hash.insert("method", d->name);
    hash.insert("timing", d->timing->toVariant());
    hash.insert("parameters", d->measurementDefinition);
    return hash;
}

TestDefinition TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinition(hash.value("id").toUuid(),
                          hash.value("method").toString(),
                          TimingFactory::timingFromVariant(hash.value("timing")),
                          hash.value("parameters"));
}

QUuid TestDefinition::id() const
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
