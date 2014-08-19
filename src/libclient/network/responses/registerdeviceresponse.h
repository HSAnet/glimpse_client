#ifndef REGISTERDEVICERESPONSE_H
#define REGISTERDEVICERESPONSE_H

#include "response.h"

class CLIENT_API RegisterDeviceResponse : public Response
{
    Q_OBJECT

public:
    RegisterDeviceResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
};

#endif // REGISTERDEVICERESPONSE_H
