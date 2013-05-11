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
