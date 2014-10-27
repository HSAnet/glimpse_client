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

Report ReportScheduler::reportByTaskId(const TaskId &taskId) const
{
    foreach (const Report &r, d->reports)
    {
        if (r.taskId() == taskId)
        {
            return r;
        }
    }

    return Report();
}

ReportList ReportScheduler::reports() const
{
    return d->reports;
}

void ReportScheduler::addReport(const Report &report)
{
    d->reports.append(report);
    emit reportAdded(report);
}

void ReportScheduler::modifyReport(const Report &report)
{
    for (int i = 0; i < d->reports.size(); ++i)
    {
        const Report &r = d->reports.at(i);

        if (r.taskId() == report.taskId())
        {
            d->reports.replace(i, report);
            emit reportModified(report);
            return;
        }
    }
}

void ReportScheduler::removeReport(const Report &report)
{
    d->reports.removeAll(report);
    emit reportRemoved(report);
}
