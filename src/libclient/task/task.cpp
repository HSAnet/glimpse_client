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
    hash.insert("id", uuidToVariant(d->id));
    hash.insert("name", d->name);
    hash.insert("timing", d->timing->toVariant());
    hash.insert("measurement_definition", d->measurementDefinition);
    return hash;
}

TestDefinitionPtr TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinitionPtr(new TestDefinition(hash.value("id").toUuid(),
                                                hash.value("name").toString(),
                                                TimingFactory::timingFromVariant(hash.value("timing")),
                                                hash.value("measurement_definition")));
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

QVariant TestDefinition::measurementDefinition() const
{
    return d->measurementDefinition;
}
