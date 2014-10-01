#include "report.h"
#include "../types.h"

#include <QUuid>

class ReportData : public QSharedData
{
public:
    TaskId taskId;
    QDateTime dateTime;
    QStringList columnLabels;
    ResultList results;
    QString appVersion;
};

Report::Report()
: d(new ReportData)
{
}

Report::Report(const Report &other)
: d(other.d)
{
}

Report::Report(const TaskId &taskId, const QDateTime &dateTime, const QString &appVersion, const QStringList columnLabels, const ResultList &results)
: d(new ReportData)
{
    d->taskId = taskId;
    d->dateTime = dateTime;
    d->appVersion = appVersion;
    d->columnLabels = QStringList() << "start_time" << "end_time" << "conflicting_tasks"
                                    << "cross_traffic" << "duration" << "measure_uuid"
                                    << "pre_info" << "post_info" << "error" << columnLabels;
    d->results = results;
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
                  map.value("column_labels").toStringList(),
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

void Report::setColumnLabels(const QStringList &columnLabels)
{
    d->columnLabels = columnLabels;
}

QStringList Report::columnLabels() const
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
    map.insert("task_id", taskId().toInt());
    map.insert("report_time", dateTime());
    map.insert("app_version", appVersion());
    map.insert("column_labels", columnLabels());
    map.insert("results", listToVariant(results()));
    return map;
}
