#include "result.h"
#include "../types.h"

Result::Result()
: d(new ResultData)
{
}

Result::Result(const QString &errorString)
: d(new ResultData)
{
    d->errorString = errorString;
}

Result::Result(const Result &other)
: d(other.d)
{
}

Result::Result(const QDateTime &startDateTime, const QDateTime &endDateTime, const QVariant &probeResult,
               const QVariant &peerResult, const QUuid &measureUuid, const QString &errorString)
: d(new ResultData)
{
    d->startDateTime = startDateTime;
    d->endDateTime = endDateTime;
    d->probeResult = probeResult;
    d->peerResult = peerResult;
    d->measureUuid = measureUuid;
    d->errorString = errorString;
}

bool Result::isNull() const
{
    return d->startDateTime.isNull() || d->endDateTime.isNull();
}

Result Result::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Result(map.value("start_date_time").toDateTime(),
                  map.value("end_date_time").toDateTime(),
                  map.value("probe_result"),
                  map.value("peer_result"),
                  map.value("measure_uuid").toUuid(),
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

QDateTime Result::endDateTime() const
{
    return d->endDateTime;
}

QVariant Result::probeResult() const
{
    return d->probeResult;
}

QVariant Result::peerResult() const
{
    return d->peerResult;
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
    map.insert("start_date_time", d->startDateTime);
    map.insert("end_date_time", d->endDateTime);
    map.insert("probe_result", d->probeResult);
    map.insert("peer_result", d->peerResult);
    map.insert("measure_uuid", uuidToString(d->measureUuid));
    map.insert("error", d->errorString);
    return map;
}
