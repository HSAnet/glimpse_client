#ifndef REQUESTS_H
#define REQUESTS_H

#include "types.h"

#include <QObject>
#include <QUuid>

class Request : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)

public:
    explicit Request(QObject* parent = 0);
    ~Request();

    virtual QVariant toVariant() const = 0;

    void setDeviceId(const QUuid& deviceId);
    QUuid deviceId() const;

    void setSessionId(const QString& sessionId);
    QString sessionId() const;

signals:
    void deviceIdChanged(const QUuid& deviceId);
    void sessionIdChanged(const QString& sessionId);

protected:
    class Private;
    friend class Private;
    Private* d;
};

class RegisterDeviceRequest : public Request
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

class ManualRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/manualrequest")

public:
    ManualRequest(QObject* parent = 0);
    ~ManualRequest();

    QVariant toVariant() const;

protected:
    class Private;
    Private* d;
};

class LoginRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/login")
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)

public:
    LoginRequest(QObject* parent = 0);
    ~LoginRequest();

    QVariant toVariant() const;

    QString password() const;
    QString userId() const;

    void setPassword(const QString& password);
    void setUserId(const QString& userId);

signals:
    void passwordChanged(const QString& password);
    void userIdChanged(const QString& userId);

protected:
    class Private;
    Private* d;
};

class UserRegisterRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/register_user")
    Q_PROPERTY(QString nameLast READ nameLast WRITE setNameLast NOTIFY nameLastChanged)
    Q_PROPERTY(QLocale::Country homeCountry READ homeCountry WRITE setHomeCountry NOTIFY homeCountryChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY mailChanged)
    Q_PROPERTY(QString nameFirst READ nameFirst WRITE setNameFirst NOTIFY nameFirstChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(int xpLevel READ xpLevel WRITE setXpLevel NOTIFY xpLevelChanged)

public:
    UserRegisterRequest(QObject* parent = 0);
    ~UserRegisterRequest();

    QVariant toVariant() const;

    QString nameLast() const;
    QLocale::Country homeCountry() const;
    QString userId() const;
    QString nameFirst() const;
    QString password() const;
    int xpLevel() const;

    void setNameLast(const QString& nameLast);
    void setHomeCountry(QLocale::Country homeCountry);
    void setUserId(const QString& userId);
    void setNameFirst(const QString& nameFirst);
    void setPassword(const QString& password);
    void setXpLevel(int xpLevel);

signals:
    void nameLastChanged(const QString& nameLast);
    void homeCountryChanged(QLocale::Country homeCountry);
    void mailChanged(const QString& userId);
    void nameFirstChanged(const QString& nameFirst);
    void passwordChanged(const QString& password);
    void xpLevelChanged(int xpLevel);

protected:
    class Private;
    Private* d;
};

#endif // REQUESTS_H
