#import "deviceinfo.h"
#include "log/logger.h"

#import <AdSupport/ASIdentifierManager.h>
#include <QCryptographicHash>

LOGGER(DeviceInfo);

DeviceInfo::DeviceInfo()
{
}

DeviceInfo::~DeviceInfo()
{
}

QString DeviceInfo::deviceId() const
{
    NSString *adId = [[[ASIdentifierManager sharedManager] advertisingIdentifier] UUIDString];

    QString advertisingIdentifier;
    advertisingIdentifier.resize([adId length]);
    [adId getCharacters:reinterpret_cast<unichar*>(advertisingIdentifier.data()) range:NSMakeRange(0, [adId length])];

    LOG_INFO(QString("Advertising Identifier: %1").arg(advertisingIdentifier));

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(advertisingIdentifier.toUtf8());

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
    return 100;
}

qint32 DeviceInfo::signalStrength() const
{
    return -1;
}

QString DeviceInfo::platform() const
{
    return "ios";
}

QString DeviceInfo::OSName() const
{
    return "";
}

QString DeviceInfo::OSVersion() const
{
    return "";
}

QString DeviceInfo::firmwareVersion() const
{
    return "";
}

QString DeviceInfo::board() const
{
    return "";
}

QString DeviceInfo::manufacturer() const
{
    return "";
}

QString DeviceInfo::model() const
{
    return "";
}

qlonglong DeviceInfo::availableDiskSpace() const
{
    return -1;
}
