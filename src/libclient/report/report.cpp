#include "report.h"
#include "../types.h"

#include <QUuid>

Report::Report()
: d(new ReportData)
{
}

Report::Report(const Report &other)
: d(other.d)
{
}

Report::Report(const quint32 &taskId, const QDateTime &dateTime, const QString &appVersion, const QStringList columnLables, const ResultList &results)
: d(new ReportData)
{
    d->taskId = taskId;
    d->dateTime = dateTime;
    d->appVersion = appVersion;
    d->columnLabels = QStringList() << "start_time" << "end_time" << "conflicting-tasks"
                                    << "cross-traffic" << "duration" << "measure_uuid" << "error" << columnLables;
    d->results = results;
}

bool Report::operator ==(const Report &other) const
{
    return (d == other.d) || (d->taskId == other.d->taskId);
}

bool Report::isNull() const
{
    return (d->taskId == 0);
}

void Report::setTaskId(const quint32 &id)
{
    d->taskId = id;
}

Report Report::fromVariant(const QVariant &variant)
{
    QVariantMap map = variant.toMap();

    return Report(map.value("task_id").toInt(),
                  map.value("report_time").toDateTime(),
                  map.value("app_version").toString(),
                  map.value("columnLables").toStringList(),
                  listFromVariant<Result>(map.value("results")));
}

quint32 Report::taskId() const
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

void Report::setColumnLabels(const QStringList &columnLables)
{
    d->columnLabels = columnLables;
}

QStringList Report::columnLables() const
{
    return d->columnLabels;
}

void Report::setResults(const ResultList &results)
{
    d->results = results;
}

ResultList Report::results() const
{
    return d->results;
}

QVariant Report::toVariant() const
{
    QVariantMap map;
    map.insert("task_id", taskId());
    map.insert("report_time", dateTime());
    map.insert("app_version", appVersion());
    map.insert("column_lables", columnLables());
    map.insert("results", listToVariant(results()));
    return map;
}
