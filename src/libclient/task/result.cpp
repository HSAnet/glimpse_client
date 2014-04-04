#include "result.h"

class Result::Private
{
public:
    QDateTime dateTime;
    QVariant probeResult;
    QVariant peerResult;
    QUuid measureUuid;
};

Result::Result(const QDateTime &dateTime, const QVariant &probeResult, const QVariant &peerResult, const QUuid measureUuid)
: d(new Private)
{
    d->dateTime = dateTime;
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

    return ResultPtr(new Result(map.value("date_time").toDateTime(),
                                map.value("probe_result"),
                                map.value("peer_result"),
                                map.value("measure_uuid").toUuid()));
}

QDateTime Result::dateTime() const
{
    return d->dateTime;
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
    map.insert("date_time", d->dateTime);
    map.insert("probe_result", d->probeResult);
    map.insert("peer_result", d->peerResult);
    map.insert("measure_uuid", d->measureUuid);
    return map;
}
