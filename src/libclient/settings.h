#ifndef SETTINGS_H
#define SETTINGS_H

#include "export.h"
#include "response.h"

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
    Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)
    Q_PROPERTY(GetConfigResponse* config READ config CONSTANT)

public:
    explicit Settings(QObject *parent = 0);
    ~Settings();

    enum StorageType {
        NewSettings,
        ExistingSettings
    };

    StorageType init();

    void setDeviceId(const QUuid& deviceId);
    QUuid deviceId() const;

    void setUserId(const QString& userId);
    QString userId() const;

    void setPassword(const QString& password);
    QString password() const;

    void setSessionId(const QString& sessionId);
    QString sessionId() const;

    GetConfigResponse* config() const;

signals:
    void deviceIdChanged(const QUuid& deviceId);
    void userIdChanged(const QString& userId);
    void passwordChanged(const QString& password);
    void sessionIdChanged(const QString& sessionId);

protected:
    class Private;
    Private* d;
};

#endif // SETTINGS_H
