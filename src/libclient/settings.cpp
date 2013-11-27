#include "settings.h"
#include "deviceinfo.h"
#include "log/logger.h"

#include <QSettings>

LOGGER(Settings);

class Settings::Private
{
public:
    GetConfigResponse config;
    QSettings settings;
};

Settings::Settings(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Settings::~Settings()
{
    d->settings.setValue("config", d->config.toVariant());
    delete d;
}

Settings::StorageType Settings::init()
{
    bool newSettings = deviceId().isNull();

    DeviceInfo info;
    setDeviceId(info.deviceId());

    LOG_INFO(QString("Device ID: %1").arg(deviceId()));

    // Create new settings
    if ( newSettings ) {
        d->config.setControllerAddress("141.82.49.82:5105");

        LOG_INFO("Created new settings for this device");

        return NewSettings;
    } else {
        d->config.fillFromVariant( qvariant_cast<QVariantMap>(d->settings.value("config")) );

        LOG_INFO("Loaded existing settings for this device");
        return ExistingSettings;
    }
}

bool Settings::hasLoginData() const
{
    return !userId().isEmpty() && !password().isEmpty();
}

void Settings::setDeviceId(const QString &deviceId)
{
    if ( this->deviceId() != deviceId ) {
        d->settings.setValue("device-id", deviceId);
        emit deviceIdChanged(deviceId);
    }
}

QString Settings::deviceId() const
{
    return d->settings.value("device-id").toString();
}

void Settings::setUserId(const QString &userId)
{
    if ( this->userId() != userId ) {
        d->settings.setValue("user-id", userId);
        emit userIdChanged(userId);
    }
}

QString Settings::userId() const
{
    return d->settings.value("user-id").toString();
}

void Settings::setPassword(const QString &password)
{
    if ( this->password() != password ) {
        d->settings.setValue("password", password);
        emit passwordChanged(password);
    }
}

QString Settings::password() const
{
    return d->settings.value("password").toString();
}

void Settings::setSessionId(const QString &sessionId)
{
    if ( this->sessionId() != sessionId ) {
        d->settings.setValue("session-id", sessionId);
        emit sessionIdChanged(sessionId);
    }
}

QString Settings::sessionId() const
{
    return d->settings.value("session-id").toString();
}

GetConfigResponse *Settings::config() const
{
    return &d->config;
}
