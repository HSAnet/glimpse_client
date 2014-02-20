#include "ondemandtiming.h"

TimingPtr OnDemandTiming::fromVariant(const QVariant &variant)
{
    Q_UNUSED(variant);
    return TimingPtr(new OnDemandTiming);
}

QString OnDemandTiming::type() const
{
    return "ondemand";
}
