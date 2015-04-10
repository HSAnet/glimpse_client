#include "timingfactory.h"
#include "periodictiming.h"
#include "onofftiming.h"
#include "calendartiming.h"
#include "immediatetiming.h"
#include "ondemandtiming.h"

TimingPtr TimingFactory::timingFromVariant(const QVariant &variant)
{
    if (!variant.isValid())
    {
        return TimingPtr();
    }

    typedef TimingPtr(*CreateFunction)(const QVariant &);
    QHash<QString, CreateFunction> factories;
    factories.insert("periodic", PeriodicTiming::fromVariant);
    factories.insert("onoff", OnOffTiming::fromVariant);
    factories.insert("calendar", CalendarTiming::fromVariant);
    factories.insert("immediate", ImmediateTiming::fromVariant);
    factories.insert("ondemand", OnDemandTiming::fromVariant);

    QVariantMap hash = variant.toMap();

    if (hash.isEmpty())
    {
        return TimingPtr();
    }

    QHashIterator<QString, CreateFunction> iter(factories);
    while (iter.hasNext())
    {
        iter.next();

        QString type = iter.key();
        QVariant value = hash.value(type);
        if (!value.isNull())
        {
            CreateFunction cf = factories.value(type);
            if (cf)
            {
                return cf(value);
            }
            else
            {
                return TimingPtr();
            }
        }
    }

    return TimingPtr();
}

TimingPtr TimingFactory::timingFromMPlaneWhen(const QString &when)
{
    if (when == "now")
    {
        return TimingPtr(new ImmediateTiming());
    }

    return TimingPtr();
}
