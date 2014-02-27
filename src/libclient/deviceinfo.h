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


protected:
    class Private;
    Private* d;
};

#endif // DEVICEINFO_H
