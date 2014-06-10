#ifndef TIMINGFACTORY_H
#define TIMINGFACTORY_H

#include "timing.h"

class CLIENT_API TimingFactory
{
public:
    static TimingPtr timingFromVariant(const QVariant &variant);
};

#endif // TIMINGFACTORY_H
