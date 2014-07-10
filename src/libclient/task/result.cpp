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

Result::Result(const QDateTime &startDateTime, const QDateTime &endDateTime, const QVariantList &probeResult, const QUuid &measureUuid, const QString &errorString)
: d(new ResultData)
{
    d->startDateTime = startDateTime;
    d->endDateTime = endDateTime;
    d->probeResult = probeResult;
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
                  map.value("probe_result").toList(),
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

void Result::setProbeResult(const QVariantList &probeResult)
{
    d->probeResult = probeResult;
}

QVariantList Result::probeResult() const
{
    return d->probeResult;
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
    list.append(d->errorString);
    list = list + d->probeResult;
    return list;
}
