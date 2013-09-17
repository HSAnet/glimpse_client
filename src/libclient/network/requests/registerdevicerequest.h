#ifndef REGISTERDEVICEREQUEST_H
#define REGISTERDEVICEREQUEST_H

#include "request.h"

class CLIENT_API RegisterDeviceRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/register_device")
    Q_ENUMS(DeviceType ProviderTechnology)
    Q_PROPERTY(QString dataPlanName READ dataPlanName WRITE setDataPlanName NOTIFY dataPlanNameChanged)
    Q_PROPERTY(ProviderTechnology providerTechnology READ providerTechnology WRITE setProviderTechnology NOTIFY providerTechnologyChanged)
    //Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString platform READ platform WRITE setPlatform NOTIFY platformChanged)
    Q_PROPERTY(DeviceType deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
    Q_PROPERTY(int maxAllowedTraffic READ maxAllowedTraffic WRITE setMaxAllowedTraffic NOTIFY maxAllowedTrafficChanged)
    Q_PROPERTY(int dataPlanDownlink READ dataPlanDownlink WRITE setDataPlanDownlink NOTIFY dataPlanDownlinkChanged)
    Q_PROPERTY(int dataPlanUplink READ dataPlanUplink WRITE setDataPlanUplink NOTIFY dataPlanUplinkChanged)
    //Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)

public:
    RegisterDeviceRequest(QObject* parent = 0);
    ~RegisterDeviceRequest();

    enum DeviceType
    {
        DeviceTypeUnknown = 0,

        Server,
        Workstation,
        Laptop,
        Phone,
        Tablet,
        Other = 100
    };

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
    //QString sessionId() const;
    QString deviceName() const;
    QString platform() const;
    DeviceType deviceType() const;
    QString provider() const;
    int maxAllowedTraffic() const;
    int dataPlanDownlink() const;
    int dataPlanUplink() const;
   // QUuid deviceId() const;

    void setDataPlanName(const QString& dataPlanName);
    void setProviderTechnology(ProviderTechnology providerTechnology);
    //void setSessionId(const QString& sessionId);
    void setDeviceName(const QString& deviceName);
    void setPlatform(const QString& platform);
    void setDeviceType(DeviceType deviceType);
    void setProvider(const QString& provider);
    void setMaxAllowedTraffic(int maxAllowedTraffic);
    void setDataPlanDownlink(int dataPlanDownlink);
    void setDataPlanUplink(int dataPlanUplink);
    //void setDeviceId(const QUuid& deviceId);

signals:
    void dataPlanNameChanged(const QString& dataPlanName);
    void providerTechnologyChanged(ProviderTechnology providerTechnology);
    //void sessionIdChanged(const QString& sessionId);
    void deviceNameChanged(const QString& deviceName);
    void platformChanged(const QString& platform);
    void deviceTypeChanged(DeviceType deviceType);
    void providerChanged(const QString& provider);
    void maxAllowedTrafficChanged(int maxAllowedTraffic);
    void dataPlanDownlinkChanged(int dataPlanDownlink);
    void dataPlanUplinkChanged(int dataPlanUplink);
    //void deviceIdChanged(const QUuid& deviceId);

protected:
    class Private;
    Private* d;
};

#endif // REGISTERDEVICEREQUEST_H
