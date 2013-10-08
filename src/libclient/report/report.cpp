#include "report.h"
#include "../types.h"

#include <QUuid>

class Report::Private
{
public:
    QUuid taskId;
    QDateTime dateTime;
    ResultList results;
};

Report::Report(const QUuid &taskId, const QDateTime &dateTime, const ResultList &results)
: d(new Private)
{
    d->taskId = taskId;
    d->dateTime = dateTime;
    d->results = results;
}

Report::~Report()
{
    delete d;
}

ReportPtr Report::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return ReportPtr(new Report(map.value("taskId").toUuid(),
                                map.value("report-time").toDateTime(),
                                ptrListFromVariant<Result>(map.value("results"))));
}

QUuid Report::taskId() const
{
    return d->taskId;
}

QDateTime Report::dateTime() const
{
    return d->dateTime;
}

ResultList Report::results() const
{
    return d->results;
}

QVariant Report::toVariant() const
{
    QVariantMap map;
    map.insert("taskId", taskId());
    map.insert("report-time", dateTime());
    map.insert("results", ptrListToVariant(results()));
    return map;
}
