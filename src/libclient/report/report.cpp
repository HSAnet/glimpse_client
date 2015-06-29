#include "report.h"
#include "../types.h"

#include <QUuid>

class ReportData : public QSharedData
{
public:
    TaskId taskId;
    QDateTime dateTime;
    ResultList results;
    QString appVersion;
    QVariantMap specification;
};

Report::Report()
: d(new ReportData)
{
}

Report::Report(const Report &other)
: d(other.d)
{
}

Report::Report(const TaskId &taskId, const QDateTime &dateTime, const QString &appVersion, const ResultList &results, const QVariantMap &specification)
: d(new ReportData)
{
    d->taskId = taskId;
    d->dateTime = dateTime;
    d->appVersion = appVersion;
    d->results = results;
    d->specification = specification;
}

Report::~Report()
{
}

Report &Report::operator=(const Report &rhs)
{
    if (this != &rhs)
    {
        d.operator=(rhs.d);
    }

    return *this;
}

bool Report::operator ==(const Report &other) const
{
    return (d == other.d) || (d->taskId == other.d->taskId);
}

bool Report::isNull() const
{
    return !d->taskId.isValid();
}

void Report::setTaskId(const TaskId &id)
{
    d->taskId = id;
}

Report Report::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Report(TaskId(map.value("task_id").toInt()),
                  map.value("report_time").toDateTime(),
                  map.value("app_version").toString(),
                  listFromVariant<Result>(map.value("results")));
}

TaskId Report::taskId() const
{
    return d->taskId;
}

void Report::setDateTime(const QDateTime &dateTime)
{
    d->dateTime = dateTime;
}

QDateTime Report::dateTime() const
{
    return d->dateTime;
}

void Report::setAppVersion(const QString &appVersion)
{
    d->appVersion = appVersion;
}

QString Report::appVersion() const
{
    return d->appVersion;
}

void Report::setResults(const ResultList &results)
{
    d->results = results;
}

ResultList Report::results() const
{
    return d->results;
}

void Report::setSpecification(const QVariantMap &specification)
{
    d->specification = specification;
}

QVariantMap Report::specification() const
{
    return d->specification;
}

QVariant Report::toVariant() const
{
    QVariantMap map = d->specification;

    // make result from specification
    map.remove("specification");
    map.insert("result", "measure");

    if (d->results.count() > 1)
    {
        map.insert("when", QString("%1 ... %2").arg(d->results.first().startDateTime().toString(Qt::ISODate).replace('T', ' ')).arg(d->results.last().endDateTime().toString(Qt::ISODate).replace('T', ' ')));
    }
    else
    {
        map.insert("when", QString("%1").arg(d->results.first().startDateTime().toString(Qt::ISODate).replace('T', ' ')));
    }

    QVariantList resList;

    foreach(const Result &res, d->results)
    {
        resList.append(res.probeResult());
    }

    map.insert("resultvalues", resList);

    return map;
}
