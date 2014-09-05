#include "deviceinfo.h"
#include "log/logger.h"
#include "client.h"
#include "network/networkmanager.h"

#include <QCryptographicHash>
#include <QDir>
#include <DiskArbitration/DADisk.h>
#include <QBatteryInfo>
#include <QNetworkInfo>
#include <QDeviceInfo>

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

QVariantMap DeviceInfo::OSInfo()
{
    QVariantMap map;
    QDeviceInfo devInfo;

    map.insert("os", devInfo.operatingSystemName());
    map.insert("os_version", devInfo.version(QDeviceInfo::Os));
    map.insert("firmware_version", devInfo.version(QDeviceInfo::Firmware));

    return map;
}
