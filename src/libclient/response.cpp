#include "response.h"
#include "client.h"
#include "settings.h"

#include "timing/timingfactory.h"
#include "channel.h"
#include "log/logger.h"

LOGGER(Response);

Response::Response(QObject *parent)
: QObject(parent)
{
}

Response::~Response()
{
}

void Response::finished()
{
}

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


RegisterDeviceResponse::RegisterDeviceResponse(QObject *parent)
: Response(parent)
{
}

bool RegisterDeviceResponse::fillFromVariant(const QVariantMap &variant)
{
    Q_UNUSED(variant);
    return true;
}


GetConfigResponse::GetConfigResponse(QObject *parent)
: Response(parent)
, m_supervisorChannel(new Channel)
, m_keepaliveChannel(new Channel)
, m_configChannel(new Channel)
, m_reportChannel(new Channel)
{
}

bool GetConfigResponse::fillFromVariant(const QVariantMap &variant)
{
    m_supervisorChannel = Channel::fromVariant(variant.value("supervisor_channel"));
    m_keepaliveChannel = Channel::fromVariant(variant.value("keepalive_channel"));
    m_configChannel = Channel::fromVariant(variant.value("config_channel"));
    m_reportChannel = Channel::fromVariant(variant.value("report_channel"));

    return true;
}

QVariant GetConfigResponse::toVariant() const
{
    QVariantMap map;
    map.insert("supervisor_channel", m_supervisorChannel->toVariant());
    map.insert("keepalive_channel", m_keepaliveChannel->toVariant());
    map.insert("config_channel", m_configChannel->toVariant());
    map.insert("report_channel", m_reportChannel->toVariant());
    return map;
}

void GetConfigResponse::finished()
{
    // Sync the settings
    Client *client = Client::instance();
    Settings *settings = client->settings();
    settings->sync();

    emit responseChanged();
}

QString GetConfigResponse::supervisorAddress() const
{
    return m_supervisorChannel->target();
}

TimingPtr GetConfigResponse::supervisorTiming() const
{
    return m_supervisorChannel->timing();
}

QString GetConfigResponse::keepaliveAddress() const
{
    return m_keepaliveChannel->target();
}

TimingPtr GetConfigResponse::keepaliveTiming() const
{
    return m_keepaliveChannel->timing();
}

void GetConfigResponse::setConfigAddress(const QString &address)
{
    m_configChannel->setTarget(address);
}

QString GetConfigResponse::configAddress() const
{
    return m_configChannel->target();
}

TimingPtr GetConfigResponse::configTiming() const
{
    return m_configChannel->timing();
}

QString GetConfigResponse::reportAddress() const
{
    return m_reportChannel->target();
}

TimingPtr GetConfigResponse::reportTiming() const
{
    return m_reportChannel->timing();
}


TestDefinitionList GetTasksResponse::tasks() const
{
    return m_tasks;
}

bool GetTasksResponse::fillFromVariant(const QVariantMap &variant)
{
    m_tasks.clear();

    QVariantList tasks = variant.value("tasks").toList();

    foreach (const QVariant &taskVariant, tasks)
    {
        TestDefinition test = TestDefinition::fromVariant(taskVariant);

        if (m_validator.validate(test) == TaskValidator::Valid)
        {
            m_tasks.append(test);
        }
        else
        {
            // TODO: Get more information
            LOG_DEBUG(QString("Received invalid task, ignoring."));
        }
    }

    LOG_DEBUG(QString("Received %1 tasks").arg(m_tasks.size()));

    return true;
}
