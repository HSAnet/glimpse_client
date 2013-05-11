#include "requests.h"

// TODO: Adjust types
#if defined(Q_OS_WIN)
#define OS "Windows"
#define TYPE ClientInfo::Workstation
#elif defined(Q_OS_ANDROID)
#define OS "Android"
#define TYPE ClientInfo::Phone
#elif defined(Q_OS_LINUX)
#define OS "Linux"
#define TYPE ClientInfo::Workstation
#elif defined(Q_OS_MAC)
#define OS "Mac OS X"
#define TYPE ClientInfo::Workstation
#endif

class Request::Private
{
public:
    QUuid deviceId;
};

Request::Request(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

Request::~Request()
{
    delete d;
}

void Request::setDeviceId(const QUuid& deviceId)
{
    if ( d->deviceId != deviceId ) {
        d->deviceId = deviceId;
        emit deviceIdChanged(deviceId);
    }
}

QUuid Request::deviceId() const
{
    return d->deviceId;
}

class ClientInfo::Private
{
public:
    Private()
    : dataPlanDownlink(0)
    , deviceType(TYPE)
    , dataPlanUplink(0)
    , maxAllowedRateUp(0)
    , maxAllowedRateDown(0)
    , userId(0)
    , os(OS)
    , remainingBudget(0)
    {
    }

    QString localIp;
    int dataPlanDownlink;
    QString upnpInfos;
    ClientInfo::DeviceType deviceType;
    QUuid deviceId;
    QString dataPlanName;
    QString provider;
    ClientInfo::ProviderTechnology providerTechnology;
    int dataPlanUplink;
    int maxAllowedRateUp;
    int maxAllowedRateDown;
    int userId;
    QString os;
    int remainingBudget;
};

ClientInfo::ClientInfo(QObject *parent)
: Request(parent)
, d(new Private)
{
}

ClientInfo::~ClientInfo()
{
    delete d;
}

int ClientInfo::dataPlanDownlink() const {
  return d->dataPlanDownlink;
}

void ClientInfo::setDataPlanDownlink(int dataPlanDownlink) {
  if (d->dataPlanDownlink != dataPlanDownlink) {
    d->dataPlanDownlink = dataPlanDownlink;
    emit dataPlanDownlinkChanged(dataPlanDownlink);
  }
}

QString ClientInfo::upnpInfos() const {
  return d->upnpInfos;
}

void ClientInfo::setUpnpInfos(const QString& upnpInfos) {
  if (d->upnpInfos != upnpInfos) {
    d->upnpInfos = upnpInfos;
    emit upnpInfosChanged(upnpInfos);
  }
}

ClientInfo::DeviceType ClientInfo::deviceType() const {
  return d->deviceType;
}

void ClientInfo::setDeviceType(DeviceType deviceType) {
  if (d->deviceType != deviceType) {
    d->deviceType = deviceType;
    emit deviceTypeChanged(deviceType);
  }
}

QString ClientInfo::dataPlanName() const {
  return d->dataPlanName;
}

void ClientInfo::setDataPlanName(const QString& dataPlanName) {
  if (d->dataPlanName != dataPlanName) {
    d->dataPlanName = dataPlanName;
    emit dataPlanNameChanged(dataPlanName);
  }
}

QString ClientInfo::provider() const {
  return d->provider;
}

void ClientInfo::setProvider(const QString& provider) {
  if (d->provider != provider) {
    d->provider = provider;
    emit providerChanged(provider);
  }
}

ClientInfo::ProviderTechnology ClientInfo::providerTechnology() const {
  return d->providerTechnology;
}

void ClientInfo::setProviderTechnology(ProviderTechnology providerTechnology) {
  if (d->providerTechnology != providerTechnology) {
    d->providerTechnology = providerTechnology;
    emit providerTechnologyChanged(providerTechnology);
  }
}

int ClientInfo::dataPlanUplink() const {
  return d->dataPlanUplink;
}

void ClientInfo::setDataPlanUplink(int dataPlanUplink) {
  if (d->dataPlanUplink != dataPlanUplink) {
    d->dataPlanUplink = dataPlanUplink;
    emit dataPlanUplinkChanged(dataPlanUplink);
  }
}

int ClientInfo::maxAllowedRateUp() const {
  return d->maxAllowedRateUp;
}

void ClientInfo::setMaxAllowedRateUp(int maxAllowedRateUp) {
  if (d->maxAllowedRateUp != maxAllowedRateUp) {
    d->maxAllowedRateUp = maxAllowedRateUp;
    emit maxAllowedRateUpChanged(maxAllowedRateUp);
  }
}

int ClientInfo::maxAllowedRateDown() const {
  return d->maxAllowedRateDown;
}

void ClientInfo::setMaxAllowedRateDown(int maxAllowedRateDown) {
  if (d->maxAllowedRateDown != maxAllowedRateDown) {
    d->maxAllowedRateDown = maxAllowedRateDown;
    emit maxAllowedRateDownChanged(maxAllowedRateDown);
  }
}

int ClientInfo::userId() const {
  return d->userId;
}

void ClientInfo::setUserId(int userId) {
  if (d->userId != userId) {
    d->userId = userId;
    emit userIdChanged(userId);
  }
}

QString ClientInfo::os() const {
  return d->os;
}

void ClientInfo::setOs(const QString& os) {
  if (d->os != os) {
    d->os = os;
    emit osChanged(os);
  }
}

int ClientInfo::remainingBudget() const {
    return d->remainingBudget;
}

void ClientInfo::setLocalIp(const QString &localIp)
{
    if (d->localIp != localIp) {
        d->localIp = localIp;
        emit localIpChanged(localIp);
    }
}

void ClientInfo::setRemainingBudget(int remainingBudget) {
  if (d->remainingBudget != remainingBudget) {
    d->remainingBudget = remainingBudget;
    emit remainingBudgetChanged(remainingBudget);
  }
}

QVariant ClientInfo::toVariant() const {
  QVariantMap data;
  data.insert("local-ip", localIp());
  data.insert("data-plan-downlink", dataPlanDownlink());
  data.insert("upnp-infos", upnpInfos());
  data.insert("device-type", enumToString(ClientInfo, "DeviceType", deviceType()));
  data.insert("device-id", deviceId());
  data.insert("data-plan-name", dataPlanName());
  data.insert("provider", provider());
  data.insert("provider-technology", enumToString(ClientInfo, "ProviderTechnology", providerTechnology()));
  data.insert("data-plan-uplink", dataPlanUplink());
  data.insert("max-allowed-rate-up", maxAllowedRateUp());
  data.insert("max-allowed-rate-down", maxAllowedRateDown());
  data.insert("user-id", userId());
  data.insert("os", os());
  data.insert("remaining-budget", remainingBudget());
  return data;
}

QString ClientInfo::localIp() const
{
    return d->localIp;
}


class ManualRequest::Private
{
public:
};

ManualRequest::ManualRequest(QObject *parent)
: Request(parent)
, d(new Private)
{
}

ManualRequest::~ManualRequest()
{
    delete d;
}

QVariant ManualRequest::toVariant() const {
  QVariantMap data;
  data.insert("device-id", deviceId());
  return data;
}
