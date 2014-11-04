#ifndef REGISTERDEVICEREQUEST_H
#define REGISTERDEVICEREQUEST_H

#include "request.h"

class CLIENT_API RegisterDeviceRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("http_request_method", "post")
    Q_CLASSINFO("authentication_method", "apikey")
    Q_ENUMS(DeviceType ProviderTechnology)
    Q_PROPERTY(QString dataPlanName READ dataPlanName WRITE setDataPlanName NOTIFY dataPlanNameChanged)
    Q_PROPERTY(ProviderTechnology providerTechnology READ providerTechnology WRITE setProviderTechnology NOTIFY
               providerTechnologyChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
    Q_PROPERTY(int maxAllowedTraffic READ maxAllowedTraffic WRITE setMaxAllowedTraffic NOTIFY maxAllowedTrafficChanged)
    Q_PROPERTY(int dataPlanDownlink READ dataPlanDownlink WRITE setDataPlanDownlink NOTIFY dataPlanDownlinkChanged)
    Q_PROPERTY(int dataPlanUplink READ dataPlanUplink WRITE setDataPlanUplink NOTIFY dataPlanUplinkChanged)

public:
    RegisterDeviceRequest(QObject *parent = 0);
    ~RegisterDeviceRequest();

    enum ProviderTechnology
    {
        ProviderTechnologyUnknown = 0,

        Ethernet,
        Cable,
        DSL
    };

    QVariant toVariant() const;

    QString dataPlanName() const;
    ProviderTechnology providerTechnology() const;
    QString deviceName() const;
    QString platform() const;
    QString provider() const;
    int maxAllowedTraffic() const;
    int dataPlanDownlink() const;
    int dataPlanUplink() const;

    void setDataPlanName(const QString &dataPlanName);
    void setProviderTechnology(ProviderTechnology providerTechnology);
    void setDeviceName(const QString &deviceName);
    void setPlatform(const QString &platform);
    void setProvider(const QString &provider);
    void setMaxAllowedTraffic(int maxAllowedTraffic);
    void setDataPlanDownlink(int dataPlanDownlink);
    void setDataPlanUplink(int dataPlanUplink);

signals:
    void dataPlanNameChanged(const QString &dataPlanName);
    void providerTechnologyChanged(ProviderTechnology providerTechnology);
    void deviceNameChanged(const QString &deviceName);
    void platformChanged(const QString &platform);
    void providerChanged(const QString &provider);
    void maxAllowedTrafficChanged(int maxAllowedTraffic);
    void dataPlanDownlinkChanged(int dataPlanDownlink);
    void dataPlanUplinkChanged(int dataPlanUplink);

protected:
    class Private;
    Private *d;
};

#endif // REGISTERDEVICEREQUEST_H
