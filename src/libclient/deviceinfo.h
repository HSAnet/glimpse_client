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

    static qreal cpuUsage();
    static quint32 freeMemory();
    static quint32 freeDiskSpace();
    static qint32 signalStrength();
    static qint8 batteryLevel();

protected:
    class Private;
    Private *d;

private:
    Q_DISABLE_COPY(DeviceInfo)
};

#endif // DEVICEINFO_H
