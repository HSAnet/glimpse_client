#ifndef TIMINGFACTORY_H
#define TIMINGFACTORY_H

#include "timing.h"

class CLIENT_API TimingFactory
{
public:
    static TimingPtr timingFromVariant(const QVariant &variant);
    static TimingPtr timingFromMPlaneWhen(const QString &variant);
    static int parseDuration(QString str);
};

#endif // TIMINGFACTORY_H
