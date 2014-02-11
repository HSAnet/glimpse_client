#ifndef ONDEMANDTIMING_H
#define ONDEMANDTIMING_H

#include "immediatetiming.h"

class CLIENT_API OnDemandTiming : public ImmediateTiming
{
public:
    static TimingPtr fromVariant(const QVariant &variant);

    QString type() const;

};

#endif // ONDEMANDTIMING_H
