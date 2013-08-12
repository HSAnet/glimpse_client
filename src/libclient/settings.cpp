#include "settings.h"

#include <QDebug>

Settings::Settings(QObject *parent)
: QObject(parent)
{
}

Settings::~Settings()
{
}

Settings::StorageType Settings::init()
{
    // Create new settings
    if ( deviceId().isNull() ) {
        setDeviceId( QUuid::createUuid() );

        qDebug() << "Created new settings for this device";

        return NewSettings;
    } else {
        qDebug() << "Loaded existing settings for this device";
        return ExistingSettings;
    }
}

void Settings::setDeviceId(const QUuid &deviceId)
{
    if ( this->deviceId() != deviceId ) {
        settings.setValue("device-id", deviceId);
        emit deviceIdChanged(deviceId);
    }
}

QUuid Settings::deviceId() const
{
    return settings.value("device-id").toUuid();
}

void Settings::setUserId(const QString &userId)
{
    if ( this->userId() != userId ) {
        settings.setValue("user-id", userId);
        emit userIdChanged(userId);
    }
}

QString Settings::userId() const
{
    return settings.value("user-id").toString();
}

void Settings::setPassword(const QString &password)
{
    if ( this->password() != password ) {
        settings.setValue("password", password);
        emit passwordChanged(password);
    }
}

QString Settings::password() const
{
    return settings.value("password").toString();
}

void Settings::setSessionId(const QString &sessionId)
{
    if ( this->sessionId() != sessionId ) {
        settings.setValue("session-id", sessionId);
        emit sessionIdChanged(sessionId);
    }
}

QString Settings::sessionId() const
{
    return settings.value("session-id").toString();
}
