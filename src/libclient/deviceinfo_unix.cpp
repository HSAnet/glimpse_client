#include "deviceinfo.h"
#include "log/logger.h"

#include <QCryptographicHash>
#include <QDir>
#include <fstab.h>

LOGGER(DeviceInfo);

namespace
{
    static const char* FSEntries[] = {
        "/",
        "/boot",
        NULL
    };
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

    for(const char** fsentry=FSEntries; *fsentry != NULL; ++fsentry)
    {
        fstab* tab = getfsfile(*fsentry);
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
            uuid.clear();
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
