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
