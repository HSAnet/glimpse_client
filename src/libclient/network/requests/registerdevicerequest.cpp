#include "registerdevicerequest.h"
#include "types.h"
#include "localinformation.h"

class RegisterDeviceRequest::Private
{
public:
    Private()
    : maxAllowedTraffic(0)
    , dataPlanDownlink(0)
    , dataPlanUplink(0)
    {
    }

    LocalInformation localInformation;
    QString dataPlanName;
    RegisterDeviceRequest::ProviderTechnology providerTechnology;
    QString deviceName;
    QString platform;
    QString provider;
    int maxAllowedTraffic;
    int dataPlanDownlink;
    int dataPlanUplink;
    QUuid deviceId;
};

RegisterDeviceRequest::RegisterDeviceRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
    setPath("/auth/api/v1/device/");
}

RegisterDeviceRequest::~RegisterDeviceRequest()
{
    delete d;
}

QString RegisterDeviceRequest::dataPlanName() const
{
    return d->dataPlanName;
}

void RegisterDeviceRequest::setDataPlanName(const QString &dataPlanName)
{
    if (d->dataPlanName != dataPlanName)
    {
        d->dataPlanName = dataPlanName;
        emit dataPlanNameChanged(dataPlanName);
    }
}

RegisterDeviceRequest::ProviderTechnology RegisterDeviceRequest::providerTechnology() const
{
    return d->providerTechnology;
}

void RegisterDeviceRequest::setProviderTechnology(ProviderTechnology providerTechnology)
{
    if (d->providerTechnology != providerTechnology)
    {
        d->providerTechnology = providerTechnology;
        emit providerTechnologyChanged(providerTechnology);
    }
}

QString RegisterDeviceRequest::deviceName() const
{
    return d->deviceName;
}

void RegisterDeviceRequest::setDeviceName(const QString &deviceName)
{
    if (d->deviceName != deviceName)
    {
        d->deviceName = deviceName;
        emit deviceNameChanged(deviceName);
    }
}

QString RegisterDeviceRequest::platform() const
{
    return d->platform;
}

void RegisterDeviceRequest::setPlatform(const QString &platform)
{
    if (d->platform != platform)
    {
        d->platform = platform;
        emit platformChanged(platform);
    }
}


QString RegisterDeviceRequest::provider() const
{
    return d->provider;
}

void RegisterDeviceRequest::setProvider(const QString &provider)
{
    if (d->provider != provider)
    {
        d->provider = provider;
        emit providerChanged(provider);
    }
}

int RegisterDeviceRequest::maxAllowedTraffic() const
{
    return d->maxAllowedTraffic;
}

void RegisterDeviceRequest::setMaxAllowedTraffic(int maxAllowedTraffic)
{
    if (d->maxAllowedTraffic != maxAllowedTraffic)
    {
        d->maxAllowedTraffic = maxAllowedTraffic;
        emit maxAllowedTrafficChanged(maxAllowedTraffic);
    }
}

int RegisterDeviceRequest::dataPlanDownlink() const
{
    return d->dataPlanDownlink;
}

void RegisterDeviceRequest::setDataPlanDownlink(int dataPlanDownlink)
{
    if (d->dataPlanDownlink != dataPlanDownlink)
    {
        d->dataPlanDownlink = dataPlanDownlink;
        emit dataPlanDownlinkChanged(dataPlanDownlink);
    }
}

int RegisterDeviceRequest::dataPlanUplink() const
{
    return d->dataPlanUplink;
}

void RegisterDeviceRequest::setDataPlanUplink(int dataPlanUplink)
{
    if (d->dataPlanUplink != dataPlanUplink)
    {
        d->dataPlanUplink = dataPlanUplink;
        emit dataPlanUplinkChanged(dataPlanUplink);
    }
}


QVariant RegisterDeviceRequest::toVariant() const
{
    QVariantMap data = d->localInformation.getConstants();

    /*data.insert("data_plan_name", dataPlanName());
    data.insert("device_name", deviceName());
    data.insert("provider_technology", enumToString(RegisterDeviceRequest, ProviderTechnology, providerTechnology()));
    data.insert("max_allowed_traffic", maxAllowedTraffic());
    data.insert("data_plan_downlink", dataPlanDownlink());
    data.insert("data_plan_uplink", dataPlanUplink());*/
    return data;
}
