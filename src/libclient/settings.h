#ifndef SETTINGS_H
#define SETTINGS_H

#include "export.h"
#include "network/responses/getconfigresponse.h"

#include <QObject>
#include <QUuid>

/**
 * The Settings class
 *
 */
class CLIENT_API Settings : public QObject
{
    Q_OBJECT
    Q_ENUMS(StorageType)
    Q_PROPERTY(QString deviceName READ deviceName WRITE setDeviceName NOTIFY deviceNameChanged)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString hashedUserId READ hashedUserId NOTIFY hashedUserIdChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(quint32 allowedTraffic READ allowedTraffic WRITE setAllowedTraffic NOTIFY allowedTrafficChanged)
    Q_PROPERTY(quint32 usedTraffic READ usedTraffic WRITE setUsedTraffic NOTIFY usedTrafficChanged)
    Q_PROPERTY(quint32 allowedMobileTraffic READ allowedMobileTraffic WRITE setAllowedMobileTraffic NOTIFY
               allowedMobileTrafficChanged)
    Q_PROPERTY(quint32 usedMobileTraffic READ usedMobileTraffic WRITE setUsedMobileTraffic NOTIFY usedMobileTrafficChanged)
    Q_PROPERTY(bool trafficBudgetManagerActive READ trafficBudgetManagerActive WRITE setTrafficBudgetManagerActive
               NOTIFY trafficBudgetManagerActiveChanged)
    Q_PROPERTY(bool mobileMeasurementsActive READ mobileMeasurementsActive WRITE setMobileMeasurementsActive
               NOTIFY mobileMeasurementsActiveChanged)
    Q_PROPERTY(quint32 backlog READ backlog WRITE setBacklog NOTIFY backlogChanged)
    Q_PROPERTY(bool googleAnalyticsActive READ googleAnalyticsActive WRITE setGoogleAnalyticsActive
               NOTIFY googleAnalyticsActiveChanged)
    Q_PROPERTY(GetConfigResponse *config READ config CONSTANT)

public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    enum StorageType
    {
        NewSettings,
        ExistingSettings
    };

    StorageType init();

    bool hasLoginData() const;

    void setDeviceName(const QString &deviceName);
    QString deviceName() const;

    void setDeviceId(const QString &deviceId);
    QString deviceId() const;

    void setUserId(const QString &userId);
    QString userId() const;
    QString hashedUserId() const;

    void setApiKey(const QString &apiKey);
    QString apiKey() const;

    bool isPassive() const;
    void setPassive(bool passive);

    void setAllowedTraffic(quint32 traffic);
    quint32 allowedTraffic() const;

    void setAllowedMobileTraffic(quint32 traffic);
    quint32 allowedMobileTraffic() const;

    void setUsedTraffic(quint32 traffic);
    quint32 usedTraffic() const;

    void setUsedMobileTraffic(quint32 traffic);
    quint32 usedMobileTraffic() const;

    void setTrafficBudgetManagerActive(bool active);
    bool trafficBudgetManagerActive() const;

    void setMobileMeasurementsActive(bool active);
    bool mobileMeasurementsActive() const;

    void setBacklog(quint32 backlog);
    quint32 backlog() const;

    void setGoogleAnalyticsActive(bool active);
    bool googleAnalyticsActive() const;

    GetConfigResponse *config() const;

    void clear();

public slots:
    void sync();

signals:
    void deviceNameChanged(const QString &deviceName);
    void deviceIdChanged(const QString &deviceId);
    void userIdChanged(const QString &userId);
    void hashedUserIdChanged(const QString &hashedUserId);
    void apiKeyChanged(const QString &apiKey);
    void passiveChanged(bool passive);
    void allowedTrafficChanged(quint32 traffic);
    void usedTrafficChanged(quint32 traffic);
    void allowedMobileTrafficChanged(quint32 traffic);
    void usedMobileTrafficChanged(quint32 traffic);
    void trafficBudgetManagerActiveChanged(bool active);
    void mobileMeasurementsActiveChanged(bool active);
    void backlogChanged(quint32 backlog);
    void googleAnalyticsActiveChanged(bool active);

protected:
    class Private;
    Private *d;
};

#endif // SETTINGS_H
