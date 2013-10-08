#include "task.h"
#include "../timing/timingfactory.h"

class TestDefinition::Private
{
public:
    QUuid id;
    QString name;
    TimingPtr timing;
};

TestDefinition::TestDefinition(const QUuid &id, const QString &name, const TimingPtr &timing)
: d(new Private)
{
    d->id = id;
    d->name = name;
    d->timing = timing;
}

TestDefinition::~TestDefinition()
{
    delete d;
}

QVariant TestDefinition::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", d->id);
    hash.insert("name", d->name);
    hash.insert("timing", d->timing->toVariant());
    return hash;
}

TestDefinitionPtr TestDefinition::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TestDefinitionPtr(new TestDefinition(hash.value("id").toUuid(),
                                                hash.value("name").toString(),
                                                TimingFactory::timingFromVariant(hash.value("timing"))));
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
