#include "deviceinfo.h"
#include "log/logger.h"

#include <QAndroidJniObject>
#include <QCryptographicHash>

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
