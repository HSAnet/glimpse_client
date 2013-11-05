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

protected:
    class Private;
    Private* d;
};

#endif // DEVICEINFO_H
