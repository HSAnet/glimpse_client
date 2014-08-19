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

    GetConfigResponse *config() const;

public slots:
    void sync();

signals:
    void deviceIdChanged(const QString &deviceId);
    void userIdChanged(const QString &userId);
    void passwordChanged(const QString &password);
    void apiKeyChanged(const QString &apiKey);
    void passiveChanged(bool passive);

protected:
    class Private;
    Private *d;
};

#endif // SETTINGS_H
