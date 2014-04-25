#include "result.h"
#include "../types.h"

class Result::Private
{
public:
    QDateTime startDateTime;
    QDateTime endDateTime;
    QVariant probeResult;
    QVariant peerResult;
    QUuid measureUuid;
};

Result::Result(const QDateTime &startDateTime, const QDateTime &endDateTime, const QVariant &probeResult,
               const QVariant &peerResult, const QUuid &measureUuid)
: d(new Private)
{
    d->startDateTime = startDateTime;
    d->endDateTime = endDateTime;
    d->probeResult = probeResult;
    d->peerResult = peerResult;
    d->measureUuid = measureUuid;
}

Result::~Result()
{
    delete d;
}

ResultPtr Result::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return ResultPtr(new Result(map.value("start_date_time").toDateTime(),
                                map.value("end_date_time").toDateTime(),
                                map.value("probe_result"),
                                map.value("peer_result"),
                                map.value("measure_uuid").toUuid()));
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

QVariant Result::toVariant() const
{
    QVariantMap map;
    map.insert("start_date_time", d->startDateTime);
    map.insert("end_date_time", d->endDateTime);
    map.insert("probe_result", d->probeResult);
    map.insert("peer_result", d->peerResult);
    map.insert("measure_uuid", uuidToString(d->measureUuid));
    return map;
}
