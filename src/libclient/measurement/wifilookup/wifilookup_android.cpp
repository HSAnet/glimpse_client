#include "wifilookup.h"
#include "../../log/logger.h"

#include <QAndroidJniObject>
#include <QAndroidJniEnvironment>
#include <QCryptographicHash>

LOGGER(DeviceInfo);

namespace
{
    QString hashBSSID(const QString &bssid)
    {
        if (bssid.length() != 17)
        {
            // bssid doesn't seem to be a valid BSSID, therefore we return
            // an empty string.
            return "";
        }

        // keep the Organization Unique Identifier
        return bssid.mid(0, 9) + QCryptographicHash::hash(
                bssid.mid(9).toUtf8(), QCryptographicHash::Sha512).toHex();
    }
}
class WifiLookup::Private
{
public:
    Private()
    : wifiLookup("de/hsaugsburg/informatik/mplane/WifiLookup")
    , accessPoints(0)
    {
    }

    QAndroidJniObject wifiLookup;
    QVector<QAndroidJniObject> accessPoints;
};

WifiLookup::WifiLookup(QObject *parent)
: Measurement(parent)
, d(new Private)
, currentStatus(Unknown)
{
}

WifiLookup::~WifiLookup()
{
    delete d;
}

Measurement::Status WifiLookup::status() const
{
    return currentStatus;
}

void WifiLookup::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool WifiLookup::prepare(NetworkManager *networkManager,
                         const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<WifiLookupDefinition>();
    return true;
}

bool WifiLookup::start()
{
    setStatus(Running);
    QAndroidJniEnvironment env;
    // getWifiList blocks for 5 seconds and returns a String array
    QAndroidJniObject wifiInfo = d->wifiLookup.callObjectMethod("getWifiList", "()[Ljava/lang/String;");
    jobjectArray objectArray = wifiInfo.object<jobjectArray>();

    for (int i = 0; i < env->GetArrayLength(objectArray); i++)
    {
        jobject element = env->GetObjectArrayElement(objectArray, i);
        d->accessPoints.append(element);
        env->DeleteLocalRef(element);
    }

    setStatus(WifiLookup::Finished);
    emit finished();
    return true;
}

bool WifiLookup::stop()
{
    setStatus(WifiLookup::Finished);
    return true;
}

Result WifiLookup::result() const
{
    QVariantList res;
    QVariantMap apRes;

    foreach (QAndroidJniObject ap, d->accessPoints)
    {
        QStringList fields = ap.toString().split(',');

        // we only need 5 of 8 fields
        if (fields.size() >= 5)
        {
            apRes.insert("ssid", QString(QCryptographicHash::hash(
                            fields.at(0).split(": ")[1].toUtf8(),
                            QCryptographicHash::Sha512).toHex()));
            apRes.insert("bssid", hashBSSID(fields.at(1).split(": ")[1]));
            apRes.insert("capabilities", fields.at(2).split(": ")[1]);
            apRes.insert("level", fields.at(3).split(": ")[1]);
            apRes.insert("frequency", fields.at(4).split(": ")[1]);

            res << apRes;
        }
    }

    QVariantMap map;
    map.insert("ap_list", res);

    return Result(map);
}
