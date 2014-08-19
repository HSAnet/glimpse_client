#include "loginresponse.h"
#include "client.h"
#include "settings.h"

LoginResponse::LoginResponse(QObject *parent)
: Response(parent)
, m_registeredDevice(false)
{
}

bool LoginResponse::fillFromVariant(const QVariantMap &variant)
{
    m_apiKey = variant.value("api_key").toString();
    m_registeredDevice = variant.value("registered_device").toBool();

    return !m_apiKey.isEmpty();
}

void LoginResponse::finished()
{
    Client *client = Client::instance();

    // Update the settings
    Settings *settings = client->settings();
    settings->setApiKey(m_apiKey);

    // Set registered status (after settings!)
    client->setStatus(Client::Registered);
}

QString LoginResponse::apiKey() const
{
    return m_apiKey;
}

bool LoginResponse::registeredDevice() const
{
    return m_registeredDevice;
}
