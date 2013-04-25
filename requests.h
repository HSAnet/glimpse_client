#ifndef REQUESTS_H
#define REQUESTS_H

#include "types.h"

#include <QObject>
#include <QUuid>

class ClientInfo : public QObject
{
    Q_OBJECT
    Q_ENUMS(DeviceType ProviderTechnology)
    Q_PROPERTY(int dataPlanDownlink READ dataPlanDownlink WRITE setDataPlanDownlink NOTIFY dataPlanDownlinkChanged)
    Q_PROPERTY(QString upnpInfos READ upnpInfos WRITE setUpnpInfos NOTIFY upnpInfosChanged)
    Q_PROPERTY(DeviceType deviceType READ deviceType WRITE setDeviceType NOTIFY deviceTypeChanged)
    Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString dataPlanName READ dataPlanName WRITE setDataPlanName NOTIFY dataPlanNameChanged)
    Q_PROPERTY(QString provider READ provider WRITE setProvider NOTIFY providerChanged)
    Q_PROPERTY(ProviderTechnology providerTechnology READ providerTechnology WRITE setProviderTechnology NOTIFY providerTechnologyChanged)
    Q_PROPERTY(int dataPlanUplink READ dataPlanUplink WRITE setDataPlanUplink NOTIFY dataPlanUplinkChanged)
    Q_PROPERTY(int maxAllowedRateUp READ maxAllowedRateUp WRITE setMaxAllowedRateUp NOTIFY maxAllowedRateUpChanged)
    Q_PROPERTY(int maxAllowedRateDown READ maxAllowedRateDown WRITE setMaxAllowedRateDown NOTIFY maxAllowedRateDownChanged)
    Q_PROPERTY(int userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString os READ os WRITE setOs NOTIFY osChanged)
    Q_PROPERTY(int remainingBudget READ remainingBudget WRITE setRemainingBudget NOTIFY remainingBudgetChanged)

public:
    ClientInfo();
    ~ClientInfo();

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

    int dataPlanDownlink() const;
    QString upnpInfos() const;
    DeviceType deviceType() const;
    QUuid deviceId() const;
    QString dataPlanName() const;
    QString provider() const;
    ProviderTechnology providerTechnology() const;
    int dataPlanUplink() const;
    int maxAllowedRateUp() const;
    int maxAllowedRateDown() const;
    int userId() const;
    QString os() const;
    int remainingBudget() const;

    void setDataPlanDownlink(int dataPlanDownlink);
    void setUpnpInfos(const QString& upnpInfos);
    void setDeviceType(DeviceType deviceType);
    void setDeviceId(const QUuid& deviceId);
    void setDataPlanName(const QString& dataPlanName);
    void setProvider(const QString& provider);
    void setProviderTechnology(ProviderTechnology providerTechnology);
    void setDataPlanUplink(int dataPlanUplink);
    void setMaxAllowedRateUp(int maxAllowedRateUp);
    void setMaxAllowedRateDown(int maxAllowedRateDown);
    void setUserId(int userId);
    void setOs(const QString& os);
    void setRemainingBudget(int remainingBudget);

signals:
    void dataPlanDownlinkChanged(int dataPlanDownlink);
    void upnpInfosChanged(const QString& upnpInfos);
    void deviceTypeChanged(DeviceType deviceType);
    void deviceIdChanged(const QUuid& deviceId);
    void dataPlanNameChanged(const QString& dataPlanName);
    void providerChanged(const QString& provider);
    void providerTechnologyChanged(ProviderTechnology providerTechnology);
    void dataPlanUplinkChanged(int dataPlanUplink);
    void maxAllowedRateUpChanged(int maxAllowedRateUp);
    void maxAllowedRateDownChanged(int maxAllowedRateDown);
    void userIdChanged(int userId);
    void osChanged(const QString& os);
    void remainingBudgetChanged(int remainingBudget);

protected:
    class Private;
    Private* d;
};

class Request : public QObject
{
    Q_OBJECT

public:
    QVariant toVariant() const;

protected:
};

#endif // REQUESTS_H
