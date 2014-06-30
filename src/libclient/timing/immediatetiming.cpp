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

bool ImmediateTiming::isValid() const
{
    return true;
}

QVariant ImmediateTiming::toVariant() const
{
    return type();
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
