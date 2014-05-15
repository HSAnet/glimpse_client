#include "timingfactory.h"
#include "periodictiming.h"
#include "onofftiming.h"
#include "calendartiming.h"
#include "immediatetiming.h"
#include "ondemandtiming.h"

TimingPtr TimingFactory::timingFromVariant(const QVariant &variant)
{
    if(!variant.isValid())
    {
        return TimingPtr();
    }

    typedef TimingPtr (*CreateFunction)(const QVariant&);
    QHash<QString, CreateFunction> factories;
    factories.insert("periodic", PeriodicTiming::fromVariant);
    factories.insert("onoff", OnOffTiming::fromVariant);
    factories.insert("calendar", CalendarTiming::fromVariant);
    factories.insert("immediate", ImmediateTiming::fromVariant);
    factories.insert("ondemand", OnDemandTiming::fromVariant);

    QVariantMap hash = variant.toMap();
    QString type = hash.keys().first();

    CreateFunction cf = factories.value(type);
    if (cf)
    {
        return cf(hash.value(type));
    }
    else
    {
        return TimingPtr();
    }
}
