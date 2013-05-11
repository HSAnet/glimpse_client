#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QUuid>

class Settings : public QObject
{
    Q_OBJECT
    Q_ENUMS(StorageType)
    Q_PROPERTY(QUuid deviceId READ deviceId WRITE setDeviceId NOTIFY deviceIdChanged)

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

signals:
    void deviceIdChanged(const QUuid& deviceId);

protected:
    QSettings settings;
};

#endif // SETTINGS_H
