#include "deviceinfo.h"
#include "log/logger.h"

#include <QCryptographicHash>
#include <QDir>
#include <fstab.h>

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
    if (int ret = setfsent() != 1) {
        LOG_ERROR(QString("Error opening fstab: setfsent returned %1").arg(ret));
        return QString();
    }

    fstab* tab = getfsfile("/");
    if (!tab) {
        LOG_ERROR("Mount point / not found!");
        endfsent();
        return QString();
    }

    QByteArray uuid = QByteArray::fromRawData(tab->fs_spec, strlen(tab->fs_spec));

    if (uuid.indexOf("UUID=") == 0)
        uuid.remove(0, 5);
    else {
        LOG_ERROR(QString("fs_spec does not contain an UUID: %1").arg(QString::fromLatin1(uuid)));
    }

    endfsent();

    LOG_INFO(QString("HDD UUID: %1").arg(QString::fromLatin1(uuid)));

    QCryptographicHash hash(QCryptographicHash::Sha224);
    hash.addData(uuid);

    return QString::fromLatin1(hash.result().toHex());
}
