#include "timingfactory.h"
#include "periodictiming.h"
#include "onofftiming.h"
#include "calendartiming.h"
#include "immediatetiming.h"

TimingPtr TimingFactory::timingFromVariant(const QVariant &variant)
{
    typedef TimingPtr (*CreateFunction)(const QVariant&);
    QHash<QString, CreateFunction> factories;
    factories.insert("periodic", PeriodicTiming::fromVariant);
    factories.insert("onoff", OnOffTiming::fromVariant);
    factories.insert("calendar", CalendarTiming::fromVariant);
    factories.insert("immediate", ImmediateTiming::fromVariant);

    QVariantMap hash = variant.toMap();
    QString type = hash.value("type").toString();

    CreateFunction cf = factories.value(type);
    if (cf)
        return cf(variant);
    else
        return TimingPtr();
}
