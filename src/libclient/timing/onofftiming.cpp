#include "onofftiming.h"

class OnOffTiming::Private
{
public:
    QDateTime dateTime;
};

OnOffTiming::OnOffTiming(const QDateTime &dateTime)
: d(new Private)
{
    d->dateTime = dateTime;
}

OnOffTiming::~OnOffTiming()
{
    delete d;
}

QString OnOffTiming::type() const
{
    return "onoff";
}

bool OnOffTiming::reset()
{
    return false;
}

QDateTime OnOffTiming::nextRun() const
{
    return d->dateTime;
}

QVariant OnOffTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("type", type());
    hash.insert("time", d->dateTime);
    return hash;
}

TimingPtr OnOffTiming::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return TimingPtr(new OnOffTiming(hash.value("time").toDateTime()));
}

QDateTime OnOffTiming::dateTime() const
{
    return d->dateTime;
}
