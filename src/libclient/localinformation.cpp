#include "localinformation.h"
#include "client.h"
#include "settings.h"
#include "measurement/upnp/upnp.h"

LocalInformation::LocalInformation()
{
}

QVariantMap LocalInformation::getVariables() const
{
    QVariantMap map;
    Settings* settings = Client::instance()->settings();

    map.insert("cpu_usage", deviceInfo.cpuUsage());
    map.insert("free_memory", deviceInfo.freeMemory());
    map.insert("signal_strength", deviceInfo.signalStrength());
    map.insert("battery_level", deviceInfo.batteryLevel());
    map.insert("available_disk_space", deviceInfo.availableDiskSpace());
    map.insert("connection_mode", networkMangager.connectionMode());
    map.insert("tbm_active", settings->trafficBudgetManagerActive());
    map.insert("available_traffic", settings->allowedTraffic());
    map.insert("available_mobile_traffic", settings->allowedMobileTraffic());
    map.insert("used_traffic", settings->usedTraffic());
    map.insert("used_mobile_traffic", settings->usedMobileTraffic());
    map.insert("mm_active", settings->mobileMeasurementsActive());

    UPnP measurement;

    measurement.start();

    return map;
}

QVariantMap LocalInformation::getConstants() const
{
    QVariantMap map;
    Settings* settings = Client::instance()->settings();

    map.insert("device_id", settings->deviceId());
    map.insert("app_version", Client::instance()->version());
    map.insert("platform", deviceInfo.platform());
    map.insert("os", deviceInfo.OSName());
    map.insert("os_version", deviceInfo.OSVersion());
    map.insert("firmware_version", deviceInfo.firmwareVersion());
    map.insert("board", deviceInfo.board());
    map.insert("manufacturer", deviceInfo.manufacturer());
    map.insert("model", deviceInfo.model());
    map.insert("name", settings->deviceName());
    //map.insert("group_id", nullptr); // set for a release if needed

    return map;
}
