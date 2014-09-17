#include "result.h"
#include "../types.h"

Result::Result()
: d(new ResultData)
{
}

Result::Result(const QString &errorString)
: d(new ResultData)
{
    setStartDateTime(QDateTime::currentDateTime());
    d->errorString = errorString;
}

Result::Result(const Result &other)
: d(other.d)
{
}

Result::Result(const QVariantList &probeResult, const QUuid &measureUuid)
: d(new ResultData)
{
    d->probeResult = probeResult;
    d->measureUuid = measureUuid;
}

Result::Result(const QDateTime &startDateTime, const QDateTime &endDateTime,
               const QVariantList &probeResult, const QUuid &measureUuid,
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

bool Result::isNull() const
{
    return d->startDateTime.isNull() || d->endDateTime.isNull();
}

Result Result::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Result(map.value("start_time").toDateTime(),
                  map.value("end_time").toDateTime(),
                  map.value("probe_result").toList(),
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

void Result::setProbeResult(const QVariantList &probeResult)
{
    d->probeResult = probeResult;
}

QVariantList Result::probeResult() const
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
    QVariantList list;
    list.append(d->startDateTime);
    list.append(d->endDateTime);
    list.append(d->conflictingTasks);
    list.append(d->crossTraffic);
    list.append(d->startDateTime.msecsTo(d->endDateTime));
    list.append(uuidToString(d->measureUuid));
    list.append(d->preInfo);
    list.append(d->postInfo);
    list.append(d->errorString);
    list = list + d->probeResult;
    return list;
}
