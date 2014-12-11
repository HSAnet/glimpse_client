#ifndef SETTINGS_H
#define SETTINGS_H

#include "export.h"
#include "network/responses/getconfigresponse.h"

#include <QObject>
#include <QUuid>

/**
 * The Settings class
 *
 * TODO: Currently we store plain passwords, this has to be changed!
 */
class CLIENT_API Settings : public QObject
{
    Q_OBJECT
    Q_ENUMS(StorageType)
    Q_PROPERTY(QString deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString apiKey READ apiKey WRITE setApiKey NOTIFY apiKeyChanged)
    Q_PROPERTY(quint32 availableTraffic READ availableTraffic WRITE setAvailableTraffic NOTIFY availableTrafficChanged)
    Q_PROPERTY(quint32 usedTraffic READ usedTraffic WRITE setUsedTraffic NOTIFY usedTrafficChanged)
    Q_PROPERTY(quint32 availableMobileTraffic READ availableMobileTraffic WRITE setAvailableMobileTraffic NOTIFY
               availableMobileTrafficChanged)
    Q_PROPERTY(quint32 usedMobileTraffic READ usedMobileTraffic WRITE setUsedMobileTraffic NOTIFY usedMobileTrafficChanged)
    Q_PROPERTY(bool trafficBudgetManagerActive READ trafficBudgetManagerActive WRITE setTrafficBudgetManagerActive
               NOTIFY trafficBudgetManagerActiveChanged)
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

    void setDeviceId(const QString &deviceId);
    QString deviceId() const;

    void setUserId(const QString &userId);
    QString userId() const;

    void setPassword(const QString &password);
    QString password() const;

    void setApiKey(const QString &apiKey);
    QString apiKey() const;

    bool isPassive() const;
    void setPassive(bool passive);

    void setAvailableTraffic(quint32 traffic);
    quint32 availableTraffic() const;

    void setAvailableMobileTraffic(quint32 traffic);
    quint32 availableMobileTraffic() const;

    void setUsedTraffic(quint32 traffic);
    quint32 usedTraffic() const;

    void setUsedMobileTraffic(quint32 traffic);
    quint32 usedMobileTraffic() const;

    void setTrafficBudgetManagerActive(bool active);
    bool trafficBudgetManagerActive() const;

    GetConfigResponse *config() const;

    void clear();

public slots:
    void sync();

signals:
    void deviceIdChanged(const QString &deviceId);
    void userIdChanged(const QString &userId);
    void passwordChanged(const QString &password);
    void apiKeyChanged(const QString &apiKey);
    void passiveChanged(bool passive);
    void availableTrafficChanged(quint32 traffic);
    void usedTrafficChanged(quint32 traffic);
    void availableMobileTrafficChanged(quint32 traffic);
    void usedMobileTrafficChanged(quint32 traffic);
    void trafficBudgetManagerActiveChanged(bool active);

protected:
    class Private;
    Private *d;
};

#endif // SETTINGS_H
