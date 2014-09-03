#include "deviceinfo.h"
#include "log/logger.h"

#include <QAndroidJniObject>
#include <QCryptographicHash>
#include <QFile>
#include <QNetworkConfigurationManager>
#include <QStringList>
#include <QThread>

#define BATTERY_SYSFS_PATH "/sys/class/power_supply/battery/"

LOGGER(DeviceInfo);

class DeviceInfo::Private
{
public:
    Private()
    : deviceInfo("de/hsaugsburg/informatik/mplane/DeviceInfo")
    {
    }

    QAndroidJniObject deviceInfo;
};

DeviceInfo::DeviceInfo()
: d(new Private)
{
}

DeviceInfo::~DeviceInfo()
{
    delete d;
}

QString DeviceInfo::deviceId() const
{
    QString androidId = d->deviceInfo.callObjectMethod<jstring>("getAndroidId").toString();
    QString buildSerial = d->deviceInfo.callObjectMethod<jstring>("getBuildSerial").toString();

    LOG_INFO(QString("Android ID: %1").arg(androidId));
    LOG_INFO(QString("Build Serial: %1").arg(buildSerial));

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(androidId.toUtf8());
    hash.addData(buildSerial.toUtf8());

    return QString::fromLatin1(hash.result().toHex());
}

qreal DeviceInfo::cpuUsage()
{
    QFile file("/proc/stat");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1.0;
    }

    QString load = file.readLine();

    QStringList toks = load.split(" ");

    long idle1 = toks[5].toLong();
    long cpu1 = toks[2].toLong() + toks[3].toLong() + toks[4].toLong()
                + toks[6].toLong() + toks[7].toLong() + toks[8].toLong();

    // sleep, this is not good (180ms at the moment)
    QThread::usleep(180000);

    file.seek(0);
    load = file.readLine();
    file.close();

    toks = load.split(" ");

    long idle2 = toks[5].toLong();
    long cpu2 = toks[2].toLong() + toks[3].toLong() + toks[4].toLong()
                + toks[6].toLong() + toks[7].toLong() + toks[8].toLong();

    return (float)(cpu2 - cpu1) / ((cpu2 + idle2) - (cpu1 + idle1));
}

qint32 DeviceInfo::wifiSNR()
{
    return 0;
}

qint8 DeviceInfo::batteryLevel()
{
    bool ok = false;

    // this gives us the percentage without needing to calculate anything
    QFile level(BATTERY_SYSFS_PATH + QStringLiteral("capacity"));

    if (level.open(QIODevice::ReadOnly))
    {
        int capacity = level.readAll().simplified().toInt(&ok);

        if (ok)
        {
            return capacity;
        }
    }

    QFile *remaining;
    QFile *maximum;

    QFile remainingCharge(BATTERY_SYSFS_PATH + QStringLiteral("charge_now"));
    QFile maximumCharge(BATTERY_SYSFS_PATH + QStringLiteral("charge_full"));
    // on some Linux systems different file are used for battery details
    QFile remainingEnergy(BATTERY_SYSFS_PATH + QStringLiteral("energy_now"));
    QFile maximumEnergy(BATTERY_SYSFS_PATH + QStringLiteral("energy_full"));

    if (!remainingCharge.open(QIODevice::ReadOnly) || !maximumCharge.open(QIODevice::ReadOnly))
    {
        if (!remainingEnergy.open(QIODevice::ReadOnly) || !maximumEnergy.open(QIODevice::ReadOnly))
        {
            return -1;
        }

        remaining = &remainingEnergy;
        maximum = &maximumEnergy;
    }
    else
    {
        remaining = &remainingCharge;
        maximum = &maximumCharge;
    }

    int capacityRemaining = remaining->readAll().simplified().toInt(&ok);

    if (!ok)
    {
        return -1;
    }

    int capacityMaximum = maximum->readAll().simplified().toInt(&ok);

    if (!ok)
    {
        return -1;
    }

    return capacityRemaining * 100 / capacityMaximum;
}
