#ifndef REPORTSCHEDULER_H
#define REPORTSCHEDULER_H

#include "report.h"

class CLIENT_API ReportScheduler : public QObject
{
    Q_OBJECT

public:
    ReportScheduler();
    ~ReportScheduler();

    Report reportByTaskId(const QUuid &taskId) const;

    ReportList reports() const;

    void addReport(const Report &report);
    void modifyReport(const Report &report); // FIXME: This should not belong here
    void removeReport(const Report &report);

signals:
    void reportAdded(const Report &report);
    void reportModified(const Report &report);
    void reportRemoved(const Report &report);

protected:
    class Private;
    Private *d;
};


#endif // REPORTSCHEDULER_H
