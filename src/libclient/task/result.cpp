#include "result.h"
#include "../types.h"
#include "client.h"
#include "controller/ntpcontroller.h"

class ResultData : public QSharedData
{
public:
    QDateTime startDateTime;
    QDateTime endDateTime;
    QVariant conflictingTasks;
    QVariant crossTraffic;
    QVariantMap probeResult;
    QUuid measureUuid;
    QVariantMap preInfo;
    QVariantMap postInfo;
    QString errorString;
};

Result::Result()
: d(new ResultData)
{
}

Result::Result(const QString &errorString)
: d(new ResultData)
{
    setStartDateTime(Client::instance()->ntpController()->currentDateTime());
    d->errorString = errorString;
}

Result::Result(const Result &other)
: d(other.d)
{
}

Result::Result(const QVariantMap &probeResult, const QUuid &measureUuid)
: d(new ResultData)
{
    d->probeResult = probeResult;
    d->measureUuid = measureUuid;
}

Result::Result(const QDateTime &startDateTime, const QDateTime &endDateTime,
               const QVariantMap &probeResult, const QUuid &measureUuid,
               const QVariantMap &preInfo, const QVariantMap &postInfo, const QString &errorString)
: d(new ResultData)
{
    d->startDateTime = startDateTime;
    d->endDateTime = endDateTime;
    d->probeResult = probeResult;
    d->measureUuid = measureUuid;
    d->preInfo = preInfo;
    d->postInfo = postInfo;
    d->errorString = errorString;
}

Result::~Result()
{
}

Result &Result::operator=(const Result &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
}

bool Result::isNull() const
{
    return d->startDateTime.isNull() || d->endDateTime.isNull();
}

Result Result::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Result(map.value("start_time").toDateTime(),
                  map.value("end_time").toDateTime(),
                  map.value("probe_result").toMap(),
                  map.value("measure_uuid").toUuid(),
                  map.value("pre_info").toMap(),
                  map.value("post_info").toMap(),
                  map.value("error").toString());
}

void Result::setStartDateTime(const QDateTime &startDateTime)
{
    d->startDateTime = startDateTime;
}

QDateTime Result::startDateTime() const
{
    return d->startDateTime;
}

void Result::setEndDateTime(const QDateTime &endDateTime)
{
    d->endDateTime = endDateTime;
}

QDateTime Result::endDateTime() const
{
    return d->endDateTime;
}

void Result::setProbeResult(const QVariantMap &probeResult)
{
    d->probeResult = probeResult;
}

QVariantMap Result::probeResult() const
{
    return d->probeResult;
}

void Result::setPreInfo(const QVariantMap &preInfo)
{
    d->preInfo = preInfo;
}

QVariantMap Result::preInfo() const
{
    return d->preInfo;
}

void Result::setPostInfo(const QVariantMap &postInfo)
{
    d->postInfo = postInfo;
}

QVariantMap Result::postInfo() const
{
    return d->postInfo;
}

QUuid Result::measureUuid() const
{
    return d->measureUuid;
}

void Result::setErrorString(const QString &errorString)
{
    d->errorString = errorString;
}

QString Result::errorString() const
{
    return d->errorString;
}

QVariant Result::toVariant() const
{
    QVariantMap map;
    map.insert("start_time", d->startDateTime);
    map.insert("end_time", d->endDateTime);
    map.insert("duration", d->startDateTime.msecsTo(d->endDateTime));
    map.insert("measure_uuid", uuidToString(d->measureUuid));
    map.insert("pre_info", d->preInfo);
    map.insert("post_info", d->postInfo);
    map.insert("error", d->errorString);
    map.insert("probe_result", d->probeResult);
    return map;
}
