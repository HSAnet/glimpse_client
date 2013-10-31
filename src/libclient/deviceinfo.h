#ifndef DEVICEINFO_H
#define DEVICEINFO_H

#include <QString>

class DeviceInfo
{
public:
    DeviceInfo();
    ~DeviceInfo();

    QString deviceId() const;

protected:
    class Private;
    Private* d;
};

#endif // DEVICEINFO_H
