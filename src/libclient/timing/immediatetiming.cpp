#include "immediatetiming.h"

class ImmediateTiming::Private
{
public:
};

ImmediateTiming::ImmediateTiming()
: d(new Private)
{
}

ImmediateTiming::~ImmediateTiming()
{
    delete d;
}

bool ImmediateTiming::reset()
{
    return false;
}

QDateTime ImmediateTiming::nextRun() const
{
    return QDateTime::currentDateTime();
}

QVariant ImmediateTiming::toVariant() const
{
    QVariantMap hash;
    hash.insert("type", type());
    return hash;
}

TimingPtr ImmediateTiming::fromVariant(const QVariant &variant)
{
    Q_UNUSED(variant);
    return TimingPtr(new ImmediateTiming);
}

QString ImmediateTiming::type() const
{
    return "immediate";
}
