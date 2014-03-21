#ifndef REPORT_H
#define REPORT_H

#include "../serializable.h"
#include "../task/result.h"

class Report;
typedef QSharedPointer<Report> ReportPtr;
typedef QList<ReportPtr> ReportList;

class CLIENT_API Report : public Serializable
{
public:
    Report(const QUuid& taskId, const QDateTime& dateTime, const QString& appVersion, const ResultList& results);
    ~Report();

    // Storage
    static ReportPtr fromVariant(const QVariant& variant);

    // Getters
    QUuid taskId() const;
    QDateTime dateTime() const;
    QString appVersion() const;
    ResultList results() const;

    // Serializable interface
    QVariant toVariant() const;

protected:
    class Private;
    Private* d;
};

Q_DECLARE_METATYPE(ReportPtr)
Q_DECLARE_METATYPE(ReportList)

#endif // REPORT_H
