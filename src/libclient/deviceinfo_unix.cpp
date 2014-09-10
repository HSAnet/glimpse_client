#include "deviceinfo.h"
#include "log/logger.h"
#include "client.h"
#include "network/networkmanager.h"

#include <QCryptographicHash>
#include <QDir>
#include <fstab.h>
#include <unistd.h>
#include <QThread>
#include <QNetworkConfigurationManager>
#include <qnetworkinfo.h>
#include <qdeviceinfo.h>
#include <qstorageinfo.h>

LOGGER(DeviceInfo);

Q_GLOBAL_STATIC_WITH_ARGS(const QString, BATTERY_SYSFS_PATH, (QLatin1String("/sys/class/power_supply/BAT%1/")))

namespace
{
    static const char *FSEntries[] =
    {
        "/",
        "/boot",
        NULL
    };

    QByteArray uuidForDevice(const QString &device)
    {
        QByteArray uuid;

        QDir byUuidDir("/dev/disk/by-uuid");

        foreach (const QFileInfo &uuidFileInfo, byUuidDir.entryInfoList(QDir::Files))
        {
            if (device == uuidFileInfo.symLinkTarget())
            {
                uuid = uuidFileInfo.fileName().toUtf8();
                break;
            }
        }

        return uuid;
    }
}

DeviceInfo::DeviceInfo()
: d(NULL)
{
}

DeviceInfo::~DeviceInfo()
{
}

QString DeviceInfo::deviceId() const
{
    if (int ret = setfsent() != 1)
    {
        LOG_ERROR(QString("Error opening fstab: setfsent returned %1").arg(ret));
        return QString();
    }

    QByteArray uuid;

    for (const char **fsentry = FSEntries; *fsentry != NULL; ++fsentry)
    {
        fstab *tab = getfsfile(*fsentry);

        if (!tab)
        {
            continue;
        }

        uuid = QByteArray::fromRawData(tab->fs_spec, strlen(tab->fs_spec));

        if (uuid.indexOf("UUID=") == 0)
        {
            uuid.remove(0, 5);
            break;
        }
        else
        {
            // No UUID in fstab
            QString target;
            QFileInfo info;

            if (uuid.indexOf("LABEL=") == 0)
            {
                uuid.remove(0, 6);
                info = QFileInfo(QString("/dev/disk/by-label/%1").arg(QString::fromUtf8(uuid)));
            }
            else
            {
                info = QFileInfo(uuid);
            }

            uuid.clear();

            if (info.isSymLink())
            {
                target = info.symLinkTarget();
            }
            else
            {
                target = info.absoluteFilePath();
            }

            uuid = uuidForDevice(target);

            if (!uuid.isEmpty())
            {
                break;
            }
        }
    }

    endfsent();

    if (uuid.isEmpty())
    {
        LOG_ERROR("No HDD UID found!");
        return QString();
    }

    LOG_INFO(QString("HDD UUID: %1").arg(QString::fromLatin1(uuid)));

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(uuid);

    return QString::fromLatin1(hash.result().toHex());
}

qreal DeviceInfo::cpuUsage() const
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

qint32 DeviceInfo::signalStrength() const
{
    return QNetworkInfo().networkSignalStrength(Client::instance()->networkManager()->connectionMode(), 0);
}

qint8 DeviceInfo::batteryLevel() const
{
    bool ok = false;

    // this gives us the percentage without needing to calculate anything
    QFile level(BATTERY_SYSFS_PATH()->arg(0) + QStringLiteral("capacity"));

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

    QFile remainingCharge(BATTERY_SYSFS_PATH()->arg(0) + QStringLiteral("charge_now"));
    QFile maximumCharge(BATTERY_SYSFS_PATH()->arg(0) + QStringLiteral("charge_full"));
    // on some Linux systems different file are used for battery details
    QFile remainingEnergy(BATTERY_SYSFS_PATH()->arg(0) + QStringLiteral("energy_now"));
    QFile maximumEnergy(BATTERY_SYSFS_PATH()->arg(0) + QStringLiteral("energy_full"));

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

    if (!ok || !capacityMaximum)
    {
        return -1;
    }

    return capacityRemaining * 100 / capacityMaximum;
}

QString DeviceInfo::OSName() const
{
    return QDeviceInfo().operatingSystemName();
}

QString DeviceInfo::OSVersion() const
{
    return QDeviceInfo().version(QDeviceInfo::Os);
}

QString DeviceInfo::firmwareVersion() const
{
    return QDeviceInfo().version(QDeviceInfo::Firmware);
}

QString DeviceInfo::board() const
{
    return QDeviceInfo().boardName();
}

QString DeviceInfo::manufacturer() const
{
    return DeviceInfo().manufacturer();
}

QString DeviceInfo::model() const
{
    return DeviceInfo().model();
}

qlonglong DeviceInfo::availableDiskSpace() const
{
    QStorageInfo info;
    qlonglong diskSpace = 0;
    /*
     * The method allLogicalDrives() lists the device's mount points.
     * Some might appear twice (e.g. "/"), others not at all (e.g. "/boot").
     * The resulting disk space may therefore be slightly inaccurate.
     */
    QStringList drives = info.allLogicalDrives();

    drives.removeDuplicates();

    foreach (const QString &drive, drives)
    {
        if (info.driveType(drive) == QStorageInfo::InternalDrive)
        {
            diskSpace += info.availableDiskSpace(drive);
        }
    }

    return diskSpace;
}
