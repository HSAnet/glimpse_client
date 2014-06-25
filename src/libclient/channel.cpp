#include "channel.h"
#include "timing/timingfactory.h"

Channel::Channel()
: d(new ChannelData)
{
}

Channel::Channel(const Channel &other)
: d(other.d)
{
}

Channel::Channel(const int &id, const QString &target, const QString &certificate, const TimingPtr &timing)
: d(new ChannelData)
{
    d->id = id;
    d->target = target;
    d->certificate = certificate;
    d->timing = timing;
}

bool Channel::isNull() const
{
    return d->id == 0;
}

Channel Channel::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return Channel(hash.value("id").toInt(),
                      hash.value("target").toString(),
                      hash.value("certificate").toString(),
                      TimingFactory::timingFromVariant(hash.value("timing")));
}

QVariant Channel::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", d->id);
    hash.insert("target", d->target);
    hash.insert("certificate", d->certificate);
    if(d->timing)
    {
        hash.insert("timing", d->timing->toVariant());
    }
    return hash;
}

void Channel::setTarget(const QString &target)
{
    d->target = target;
}

QString Channel::target() const
{
    return d->target;
}

void Channel::setTiming(const TimingPtr &timing)
{
    d->timing = timing;
}

TimingPtr Channel::timing() const
{
    return d->timing;
}
