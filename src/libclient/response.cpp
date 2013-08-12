#include "response.h"

Response::Response(QObject *parent)
: QObject(parent)
{
}

Response::~Response()
{
}

QString Response::errorText() const
{
    return m_errorText;
}

LoginResponse::LoginResponse(QObject *parent)
: Response(parent)
{
}

bool LoginResponse::fillFromVariant(const QVariantMap &variant)
{
    m_sessionId = variant.value("session_id").toString();
    m_registeredDevice = variant.value("registered_device", true).toBool();

    emit responseChanged();

    return !m_sessionId.isEmpty();
}

QString LoginResponse::sessionId() const
{
    return m_sessionId;
}

bool LoginResponse::registeredDevice() const
{
    return m_registeredDevice;
}


RegisterDeviceResponse::RegisterDeviceResponse(QObject *parent)
: Response(parent)
{
}

bool RegisterDeviceResponse::fillFromVariant(const QVariantMap &variant)
{
    Q_UNUSED(variant);
    return true;
}
