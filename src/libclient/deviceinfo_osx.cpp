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

    LOG_INFO(QString("HDD UUID: %1").arg(uuid));

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(uuid.toLatin1());

    return QString::fromLatin1(hash.result().toHex());
}

qreal DeviceInfo::cpuUsage()
{
    return -1.0;
}

qint8 DeviceInfo::batteryLevel()
{
    return QBatteryInfo().level();
}

qint32 DeviceInfo::signalStrength()
{
    return QNetworkInfo().networkSignalStrength(Client::instance()->networkManager()->connectionMode(), 0);
}

QString DeviceInfo::OSName()
{
    return QDeviceInfo().operatingSystemName();
}

QString DeviceInfo::OSVersion()
{
    return QDeviceInfo().version(QDeviceInfo::Os);
}

QString DeviceInfo::firmwareVersion()
{
    return QDeviceInfo().version(QDeviceInfo::Firmware);
}

QString DeviceInfo::board()
{
    return QDeviceInfo().boardName();
}

QString DeviceInfo::manufacturer()
{
    return DeviceInfo().manufacturer();
}

QString DeviceInfo::model()
{
    return DeviceInfo().model();
}

qlonglong DeviceInfo::availableDiskSpace()
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
