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
    TimingPtr timing;
};

Report::Report()
: d(new ReportData)
{
}

Report::Report(const Report &other)
: d(other.d)
{
}

Report::Report(const TaskId &taskId, const QDateTime &dateTime, const QString &appVersion, const ResultList &results, const QVariantMap &specification, const TimingPtr &timing)
: d(new ReportData)
{
    d->taskId = taskId;
    d->dateTime = dateTime;
    d->appVersion = appVersion;
    d->results = results;
    d->specification = specification;
    d->timing = timing;
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

void Report::setTiming(const TimingPtr &timing)
{
    d->timing = timing;
}

TimingPtr Report::timing() const
{
    return d->timing;
}

QVariant Report::toVariant() const
{    
    QVariantMap envelope = d->specification;
    QStringList errors;

    // make result from specification
    envelope.remove("specification");
    envelope.remove("registry");
    envelope.remove("parameters");
    envelope.remove("results");
    envelope.remove("link");
    envelope.insert("envelope", "message");

    QVariantList contents;

    if (d->results.count() > 1)
    {
        envelope.insert("when", QString("%1 ... %2").arg(d->results.first().startDateTime().toString(Qt::ISODate).replace('T', ' ')).arg(d->results.last().endDateTime().toString(Qt::ISODate).replace('T', ' ')));
    }
    else
    {
        envelope.insert("when", QString("%1").arg(d->results.first().startDateTime().toString(Qt::ISODate).replace('T', ' ')));
    }

    foreach(const Result &res, d->results)
    {
        QVariantMap result = d->specification;
        result.remove("specification");
        result.remove("token");
        result.remove("link");
        result.insert("result", "measure");
        QString token = QUuid::createUuid().toString();
        result.insert("token", token.mid(1, token.length()-2).remove('-'));
        result.insert("when", QString("%1 ... %2").arg(res.startDateTime().toString(Qt::ISODate).replace('T', ' ')).arg(res.endDateTime().toString(Qt::ISODate).replace('T', ' ')));
        result.insert("resultvalues", res.probeResult());

        if (!res.errorString().isEmpty())
        {
            errors.append(res.errorString());
        }
        contents.append(result);
    }

    // check if we have results at all
    if (contents.isEmpty())
    {
        // generate exception
        // TODO this does not happen at the moment, do we need this?
        QVariantMap exception;
        exception.insert("exception", envelope.value("token").toString());
        exception.insert("message", "One or multiple errors: " + errors.join(','));
    }
    else
    {
        envelope.insert("contents", contents);
        if (!errors.isEmpty())
        {
            envelope.insert("metadata", "One or multiple executions not possible due to the following error(s): " + errors.join(','));
        }
    }

    return envelope;
}
