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
    , columnLabels(other.columnLabels)
    , results(other.results)
    , appVersion(other.appVersion)
    {
    }

    quint32 taskId;
    QDateTime dateTime;
    QStringList columnLabels;
    ResultList results;
    QString appVersion;
};


class CLIENT_API Report : public Serializable
{
public:
    Report();
    Report(const Report &other);
    Report(const quint32 &taskId, const QDateTime &dateTime, const QString &appVersion, const QStringList columnLables, const ResultList &results);

    bool operator ==(const Report &other) const;

    bool isNull() const;

    // Getters
    void setTaskId(const quint32 &id);
    quint32 taskId() const;

    void setDateTime(const QDateTime &dateTime);
    QDateTime dateTime() const;

    void setAppVersion(const QString &appVersion);
    QString appVersion() const;

    void setColumnLabels(const QStringList &columnLables);
    QStringList columnLables() const;

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
Q_DECLARE_TYPEINFO(Report, Q_MOVABLE_TYPE);
Q_DECLARE_METATYPE(ReportList)

#endif // REPORT_H
