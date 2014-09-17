#ifndef LOCALINFORMATION_H
#define LOCALINFORMATION_H

#include "deviceinfo.h"
#include "network/networkmanager.h"

#include <QVariant>


class LocalInformation
{
public:
    LocalInformation();

    QVariantMap getVariables() const;
    QVariantMap getConstants() const;

private:
    DeviceInfo deviceInfo;
    NetworkManager networkMangager;
};

#endif // LOCALINFORMATION_H
