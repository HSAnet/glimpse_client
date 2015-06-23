#ifndef CAPABILITYRESPONSE_H
#define CAPABILITYRESPONSE_H

#include "response.h"

class CapabilityResponse : public Response
{
    Q_OBJECT
public:
    CapabilityResponse(QObject *parent = 0);
    ~CapabilityResponse();

    bool fillFromVariant(const QVariantMap &variant);

protected:
    class Private;
    friend class Private;
    Private *d;
};

#endif // CAPABILITYRESPONSE_H
