#ifndef TIMINGFACTORY_H
#define TIMINGFACTORY_H

#include "timing.h"

class TimingFactory
{
public:
    static TimingPtr timingFromVariant(const QVariant& variant);
};

#endif // TIMINGFACTORY_H
