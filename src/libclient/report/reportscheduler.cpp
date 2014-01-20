#include "reportscheduler.h"

#include <QUuid>

class ReportScheduler::Private
{
public:
    ReportList reports;
};

ReportScheduler::ReportScheduler()
: d(new Private)
{
}

ReportScheduler::~ReportScheduler()
{
    delete d;
}

ReportPtr ReportScheduler::reportByTaskId(const QUuid &taskId) const
{
    foreach(const ReportPtr& r, d->reports)
    {
        if (r->taskId() == taskId)
        {
            return r;
        }
    }

    return ReportPtr();
}

ReportList ReportScheduler::reports() const
{
    return d->reports;
}

void ReportScheduler::addReport(const ReportPtr &report)
{
    d->reports.append(report);
    emit reportAdded(report);
}

void ReportScheduler::modifyReport(const ReportPtr &report)
{
    for (int i=0; i < d->reports.size(); ++i)
    {
        const ReportPtr& r = d->reports.at(i);

        if (r->taskId() == report->taskId())
        {
            d->reports.replace(i, report);
            emit reportModified(report);
            return;
        }
    }
}

void ReportScheduler::removeReport(const ReportPtr &report)
{
    d->reports.removeAll(report);
    emit reportRemoved(report);
}
