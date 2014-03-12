#include "deviceinfo.h"
#include "log/logger.h"

#include <QCryptographicHash>
#include <QDir>
#include <fstab.h>
#include <unistd.h>
#include <QThread>
#include <QNetworkConfigurationManager>

LOGGER(DeviceInfo);

namespace
{
    static const char* FSEntries[] =
    {
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

    for (const char** fsentry=FSEntries; *fsentry != NULL; ++fsentry)
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

qint32 DeviceInfo::wifiSNR() const
{
    return 0;
}
