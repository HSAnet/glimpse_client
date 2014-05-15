#include "channel.h"
#include "timing/timingfactory.h"

class Channel::Private
{
public:
    int id;
    QString target;
    QString certificate;
    TimingPtr timing;
};

Channel::Channel()
: d(new Private)
{
    d->id = 0;
    d->target = QString();
    d->certificate = QString();
    d->timing = TimingPtr();
}

Channel::Channel(int id, QString target, QString certificate, TimingPtr timing)
: d(new Private)
{
    d->id = id;
    d->target = target;
    d->certificate = certificate;
    d->timing = timing;
}

Channel::~Channel()
{
    delete d;
}

ChannelPtr Channel::fromVariant(const QVariant &variant)
{
    QVariantMap hash = variant.toMap();

    return ChannelPtr(new Channel(hash.value("id").toInt(),
                                  hash.value("target").toString(),
                                  hash.value("certificate").toString(),
                                  TimingFactory::timingFromVariant(hash.value("timing"))));
}

QVariant Channel::toVariant() const
{
    QVariantMap hash;
    hash.insert("id", d->id);
    hash.insert("target", d->target);
    hash.insert("certificate", d->certificate);
    hash.insert("timing", d->timing->toVariant());
    return hash;
}

void Channel::setTarget(const QString target)
{
    d->target = target;
}

QString Channel::target() const
{
    return d->target;
}

TimingPtr Channel::timing() const
{
    return d->timing;
}
