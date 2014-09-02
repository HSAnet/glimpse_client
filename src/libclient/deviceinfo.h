#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>

class DeviceInfo
{
public:
    DeviceInfo();
    ~DeviceInfo();

    /// @returns A sha-224 hash or an empty string on error
    QString deviceId() const;

    qreal cpuUsage() const;
    quint32 freeMemory() const;
    quint32 freeDiskSpace() const;
    qint32 wifiSNR() const;
    static quint8 batteryLevel();

protected:
    class Private;
    Private *d;

private:
    Q_DISABLE_COPY(DeviceInfo)
};

#endif // DEVICEINFO_H
