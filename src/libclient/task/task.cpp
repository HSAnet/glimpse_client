#include "task.h"
#include "../types.h"
#include "../timing/timingfactory.h"

class TestDefinition::Private
{
public:
    QUuid id;
    QString name;
    TimingPtr timing;
    QVariant measurementDefinition;
};

TestDefinition::TestDefinition(const QUuid &id, const QString &name, const TimingPtr &timing, const QVariant &measurementDefinition)
: d(new Private)
{
    d->id = id;
    d->name = name;
    d->timing = timing;
    d->measurementDefinition = measurementDefinition;
}

TestDefinition::~TestDefinition()
{
    delete d;
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

TestDefinitionPtr TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinitionPtr(new TestDefinition(hash.value("id").toUuid(),
                                                hash.value("method").toString(),
                                                TimingFactory::timingFromVariant(hash.value("timing")),
                                                hash.value("parameters")));
}

QUuid TestDefinition::id() const
{
    return d->id;
}

QString TestDefinition::name() const
{
    return d->name;
}

TimingPtr TestDefinition::timing() const
{
    return d->timing;
}

void TestDefinition::setTiming(TimingPtr &timing)
{
    d->timing = timing;
}

QVariant TestDefinition::measurementDefinition() const
{
    return d->measurementDefinition;
}
