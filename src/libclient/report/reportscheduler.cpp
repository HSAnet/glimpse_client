#include "reportscheduler.h"

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

ReportList ReportScheduler::reports() const
{
    return d->reports;
}

void ReportScheduler::addReport(const ReportPtr &report)
{
    d->reports.append(report);
    emit reportAdded(report);
}

void ReportScheduler::removeReport(const ReportPtr &report)
{
    d->reports.removeAll(report);
    emit reportRemoved(report);
}
