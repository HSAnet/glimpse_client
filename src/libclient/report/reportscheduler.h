#ifndef REPORTSCHEDULER_H
#define REPORTSCHEDULER_H

#include "report.h"

class CLIENT_API ReportScheduler : public QObject
{
    Q_OBJECT

public:
    ReportScheduler();
    ~ReportScheduler();

    ReportPtr reportByTaskId(const QUuid& taskId) const;

    ReportList reports() const;

    void addReport(const ReportPtr& report);
    void modifyReport(const ReportPtr& report); // FIXME: This should not belong here
    void removeReport(const ReportPtr& report);

signals:
    void reportAdded(const ReportPtr& report);
    void reportModified(const ReportPtr& report);
    void reportRemoved(const ReportPtr& report);

protected:
    class Private;
    Private* d;
};


#endif // REPORTSCHEDULER_H
