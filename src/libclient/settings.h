#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QUuid>

/**
 * The Settings class
 *
 * TODO: Currently we store plain passwords, this has to be changed!
 */
class Settings : public QObject
{
    Q_OBJECT
    Q_ENUMS(StorageType)
    Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)
    Q_PROPERTY(QString userId READ userId WRITE setUserId NOTIFY userIdChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)
    Q_PROPERTY(QString sessionId READ sessionId WRITE setSessionId NOTIFY sessionIdChanged)

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

signals:
    void deviceIdChanged(const QUuid& deviceId);
    void userIdChanged(const QString& userId);
    void passwordChanged(const QString& password);
    void sessionIdChanged(const QString& sessionId);

protected:
    QSettings settings;
};

#endif // SETTINGS_H
