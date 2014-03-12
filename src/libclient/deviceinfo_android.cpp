#include "deviceinfo.h"
#include "log/logger.h"

#include <QAndroidJniObject>
#include <QCryptographicHash>
#include <QFile>
#include <QNetworkConfigurationManager>
#include <QStringList>
#include <QThread>

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
