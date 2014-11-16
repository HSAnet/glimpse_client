#include "deviceinfo.h"
#include "log/logger.h"
#include "client.h"
#include "network/networkmanager.h"

#include <QCryptographicHash>
#include <QDir>
#include <DiskArbitration/DADisk.h>
#include <qbatteryinfo.h>
#include <qnetworkinfo.h>
#include <qdeviceinfo.h>
#include <qstorageinfo.h>

LOGGER(DeviceInfo);

DeviceInfo::DeviceInfo()
: d(NULL)
{
}

DeviceInfo::~DeviceInfo()
{
}

QString DeviceInfo::deviceId() const
{
    QString uuid;

    DADiskRef disk;
    CFDictionaryRef descDict;
    DASessionRef session = DASessionCreate(NULL);

    if (session)
    {
        char *mountPoint = "/";
        CFURLRef url = CFURLCreateFromFileSystemRepresentation(NULL, (const UInt8 *)mountPoint, strlen(mountPoint), TRUE);
        disk = DADiskCreateFromVolumePath(NULL, session, url);
        CFRelease(url);

        if (disk)
        {
            descDict = DADiskCopyDescription(disk);

            if (descDict)
            {
                CFTypeRef value = (CFTypeRef)CFDictionaryGetValue(descDict,
                                                                  CFSTR("DAVolumeUUID"));
                CFStringRef strValue = CFStringCreateWithFormat(NULL, NULL,
                                                                CFSTR("%@"), value);

                uuid = QString::fromCFString(strValue);

                CFRelease(strValue);
                CFRelease(descDict);
            }

            CFRelease(disk);
        }

        CFRelease(session);
    }

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(uuid.toLatin1());

    return QString::fromLatin1(hash.result().toHex());
}

qreal DeviceInfo::cpuUsage() const
{
    return -1.0;
}

quint32 DeviceInfo::freeMemory() const
{
    return 0;
}

qint8 DeviceInfo::batteryLevel() const
{
    return QBatteryInfo().level();
}

qint32 DeviceInfo::signalStrength() const
{
    return QNetworkInfo().networkSignalStrength(Client::instance()->networkManager()->connectionMode(), 0);
}

QString DeviceInfo::platform() const
{
    return "osx";
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
    return QDeviceInfo().manufacturer();
}

QString DeviceInfo::model() const
{
    return QDeviceInfo().model();
}

qlonglong DeviceInfo::availableDiskSpace() const
{
    QStorageInfo info;
    qlonglong diskSpace = 0;
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
