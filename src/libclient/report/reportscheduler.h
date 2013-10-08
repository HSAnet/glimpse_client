#ifndef REPORTSCHEDULER_H
#define REPORTSCHEDULER_H

#include "report.h"

class CLIENT_API ReportScheduler : public QObject
{
    Q_OBJECT

public:
    ReportScheduler();
    ~ReportScheduler();

    Report reportByTaskId(const QUuid& taskId) const;

    ReportList reports() const;

    void addReport(const ReportPtr& report);
    void removeReport(const ReportPtr& report);

signals:
    void reportAdded(const ReportPtr& report);
    void reportRemoved(const ReportPtr& report);

protected:
    class Private;
    Private* d;
};


#endif // REPORTSCHEDULER_H
