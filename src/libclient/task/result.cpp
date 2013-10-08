#include "result.h"

class Result::Private
{
public:
    QDateTime dateTime;
    QString probeResult;
    QString peerResult;
};

Result::Result(const QDateTime &dateTime, const QString &probeResult, const QString &peerResult)
: d(new Private)
{
    d->dateTime = dateTime;
    d->probeResult = probeResult;
    d->peerResult = peerResult;
}

Result::~Result()
{
    delete d;
}

ResultPtr Result::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return ResultPtr(new Result(map.value("dateTime").toDateTime(),
                                map.value("probe_result").toString(),
                                map.value("peer_result").toString()));
}

QDateTime Result::dateTime() const
{
    return d->dateTime;
}

QString Result::probeResult() const
{
    return d->probeResult;
}

QString Result::peerResult() const
{
    return d->peerResult;
}

QVariant Result::toVariant() const
{
    QVariantMap map;
    map.insert("dateTime", d->dateTime);
    map.insert("probe_result", d->probeResult);
    map.insert("peer_result", d->peerResult);
    return map;
}
