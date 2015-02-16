#include "settings.h"
#include "deviceinfo.h"
#include "log/logger.h"
#include "types.h"

#include <QSettings>
#include <QCryptographicHash>

static QLatin1String CONFIG_SERVER("supervisor.measure-it.net");
static QLatin1String SUPERVISOR_SERVER("supervisor.measure-it.net");

LOGGER(Settings);

class Settings::Private
{
public:
    void sync();

    GetConfigResponse config;
    QSettings settings;
};

void Settings::Private::sync()
{
    settings.setValue("config", config.toVariant());

     // remove password before sync so it is not written into storage
    QVariant tmpPassword = settings.value("password");
    settings.remove("password");
    settings.sync();
    settings.setValue("password", tmpPassword);
}

Settings::Settings(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Settings::~Settings()
{
    d->sync();
    delete d;
}

Settings::StorageType Settings::init()
{
    bool newSettings = deviceId().isNull();

    DeviceInfo info;

    if (deviceId().isEmpty())
    {
        QString generatedDeviceId = info.deviceId();

        if (generatedDeviceId.isEmpty())
        {
            QCryptographicHash hash(QCryptographicHash::Sha224);
            hash.addData(QUuid::createUuid().toByteArray());

            setDeviceId(QString::fromLatin1(hash.result().toHex()));

            LOG_INFO("Generated fallback device ID");
        }
        else
        {
            setDeviceId(generatedDeviceId);
        }
    }

    LOG_INFO(QString("Device ID: %1").arg(deviceId()));

    // Create new settings
    if (newSettings)
    {
        d->config.setConfigAddress(CONFIG_SERVER);
        d->config.setSupervisorAdress(SUPERVISOR_SERVER);
        setUsedTraffic(0);
        setUsedMobileTraffic(0);
        LOG_DEBUG("Created new settings for this device");

        return NewSettings;
    }
    else
    {
        d->config.fillFromVariant(qvariant_cast<QVariantMap>(d->settings.value("config")));

        // Always set the controller address if we have none
        if (d->config.configAddress().isEmpty())
        {
            LOG_WARNING("Config controller address lost, setting back default one");
            d->config.setConfigAddress(SUPERVISOR_SERVER);
        }

        if (d->config.supervisorAddress().isEmpty())
        {
            LOG_WARNING("Supervisor address lost, setting back default one");
            d->config.setConfigAddress(CONFIG_SERVER);
        }

        LOG_DEBUG("Loaded existing settings for this device");
        return ExistingSettings;
    }
}

bool Settings::hasLoginData() const
{
    return !userId().isEmpty() && !apiKey().isEmpty();
}

void Settings::setDeviceId(const QString &deviceId)
{
    if (this->deviceId() != deviceId)
    {
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
    QString hash = userIdToHash(userId);
    if (this->userId() != hash)
    {
        d->settings.setValue("user-id", hash);
        emit userIdChanged(hash);
    }
}

QString Settings::userId() const
{
    return d->settings.value("user-id").toString();
}

void Settings::setPassword(const QString &password)
{
    if (this->password() != password)
    {
        d->settings.setValue("password", password);
        emit passwordChanged(password);
    }
}

QString Settings::password() const
{
    return d->settings.value("password").toString();
}

void Settings::setApiKey(const QString &apiKey)
{
    if (this->apiKey() != apiKey)
    {
        d->settings.setValue("api-key", apiKey);
        emit apiKeyChanged(apiKey);
    }
}

QString Settings::apiKey() const
{
    return d->settings.value("api-key").toString();
}

bool Settings::isPassive() const
{
    return d->settings.value("passive").toBool();
}

void Settings::setPassive(bool passive)
{
    if (this->isPassive() != passive)
    {
        d->settings.setValue("passive", passive);
        emit passiveChanged(passive);
    }
}

void Settings::setAvailableTraffic(quint32 traffic)
{
    if (this->availableTraffic() != traffic)
    {
        d->settings.setValue("available-traffic", traffic);
        emit availableTrafficChanged(traffic);
    }
}

quint32 Settings::availableTraffic() const
{
    return d->settings.value("available-traffic", 31457280).toUInt(); // defaults to 30 MB
}

void Settings::setAvailableMobileTraffic(quint32 traffic)
{
    if (this->availableMobileTraffic() != traffic)
    {
        d->settings.setValue("available-mobile-traffic", traffic);
        emit availableMobileTrafficChanged(traffic);
    }
}

quint32 Settings::availableMobileTraffic() const
{
    return d->settings.value("available-mobile-traffic", 20971520).toUInt(); // defaults to 20 MB
}

void Settings::setUsedTraffic(quint32 traffic)
{
    if (this->usedTraffic() != traffic)
    {
        d->settings.setValue("used-traffic", traffic);
        emit usedTrafficChanged(traffic);
    }
}

quint32 Settings::usedTraffic() const
{
    return d->settings.value("used-traffic", 0).toUInt();
}

void Settings::setUsedMobileTraffic(quint32 traffic)
{
    if (this->usedMobileTraffic() != traffic)
    {
        d->settings.setValue("used-mobile-traffic", traffic);
        emit usedMobileTrafficChanged(traffic);
    }
}

quint32 Settings::usedMobileTraffic() const
{
    return d->settings.value("used-mobile-traffic", 0).toUInt();
}

void Settings::setTrafficBudgetManagerActive(bool active)
{
    if (this->trafficBudgetManagerActive() != active)
    {
        d->settings.setValue("traffic-budget-manager-active", active);
        emit trafficBudgetManagerActiveChanged(active);
    }
}

bool Settings::trafficBudgetManagerActive() const
{
    return d->settings.value("traffic-budget-manager-active", false).toBool();
}

GetConfigResponse *Settings::config() const
{
    return &d->config;
}

void Settings::clear()
{
    d->settings.clear();
    d->settings.sync();
}

void Settings::sync()
{
    d->sync();
}
