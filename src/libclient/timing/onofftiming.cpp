#include "onofftiming.h"

Q_GLOBAL_STATIC(Ntp, ntp)

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

QDateTime OnOffTiming::nextRun(const QDateTime &tzero) const
{
    Q_UNUSED(tzero)
    return d->dateTime.addSecs(ntp->offset());
}

bool OnOffTiming::isValid() const
{
    return d->dateTime.isValid();
}

QVariant OnOffTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("time", d->dateTime);

    QVariantMap resultMap;
    resultMap.insert(type(), hash);
    return resultMap;
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
