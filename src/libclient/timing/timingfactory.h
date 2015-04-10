#ifndef TIMINGFACTORY_H
#define TIMINGFACTORY_H

#include "timing.h"

class CLIENT_API TimingFactory
{
public:
    static TimingPtr timingFromVariant(const QVariant &variant);
    static TimingPtr timingFromMPlaneWhen(const QString &variant);
};

#endif // TIMINGFACTORY_H
