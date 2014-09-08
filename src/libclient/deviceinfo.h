#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>
#include <QVariantMap>

class DeviceInfo
{
public:
    DeviceInfo();
    ~DeviceInfo();

    /// @returns A sha-224 hash or an empty string on error
    QString deviceId() const;

#if !defined(Q_OS_ANDROID)
    static qreal cpuUsage();
    static quint32 freeMemory();
    static quint32 freeDiskSpace();
    static qint32 signalStrength();
    static qint8 batteryLevel();
    static QVariantMap OSInfo();
    static QVariantMap HWInfo();
    static qlonglong availableDiskSpace();
#else
    qreal cpuUsage() const;
    quint32 freeMemory() const;
    quint32 freeDiskSpace() const;
    qint32 signalStrength() const;
    qint8 batteryLevel() const;
    QString OSName() const;
    QString OSVersion() const;
    QString firmwareVersion() const;
    QString board() const;
    QString manufacturer() const;
    QString model() const;
    qlonglong availableDiskSpace() const;
#endif

protected:
    class Private;
    Private *d;

private:
    Q_DISABLE_COPY(DeviceInfo)
};

#endif // DEVICEINFO_H
