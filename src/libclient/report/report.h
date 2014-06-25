#ifndef REPORT_H
#define REPORT_H

#include "../serializable.h"
#include "../task/result.h"

class Report;
typedef QList<Report> ReportList;


class ReportData : public QSharedData
{
public:
    ReportData()
    {
    }

    ReportData(const ReportData &other)
    : QSharedData(other)
    , taskId(other.taskId)
    , dateTime(other.dateTime)
    , results(other.results)
    , appVersion(other.appVersion)
    {
    }

    QUuid taskId;
    QDateTime dateTime;
    ResultList results;
    QString appVersion;
};


class CLIENT_API Report : public Serializable
{
public:
    Report();
    Report(const Report &other);
    Report(const QUuid &taskId, const QDateTime &dateTime, const QString &appVersion, const ResultList &results);

    bool operator ==(const Report &other) const;

    bool isNull() const;

    // Getters
    void setTaskId(const QUuid &id);
    QUuid taskId() const;

    void setDateTime(const QDateTime &dateTime);
    QDateTime dateTime() const;

    void setAppVersion(const QString &appVersion);
    QString appVersion() const;

    void setResults(const ResultList &results);
    ResultList results() const;

    // Storage
    static Report fromVariant(const QVariant &variant);

    // Serializable interface
    QVariant toVariant() const;

private:
    QSharedDataPointer<ReportData> d;
};

Q_DECLARE_METATYPE(Report)
Q_DECLARE_METATYPE(ReportList)

#endif // REPORT_H
