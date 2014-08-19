#ifndef LOGINRESPONSE_H
#define LOGINRESPONSE_H

#include "response.h"

class CLIENT_API LoginResponse : public Response
{
    Q_OBJECT

public:
    LoginResponse(QObject *parent = 0);

    bool fillFromVariant(const QVariantMap &variant);
    void finished();

    QString apiKey() const;
    bool registeredDevice() const;

protected:
    QString m_apiKey;
    bool m_registeredDevice;
};

typedef LoginResponse RegisterUserResponse;

#endif // LOGINRESPONSE_H
