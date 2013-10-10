#include "reportstorage.h"

#include <QPointer>
#include <QDir>
#include <QCoreApplication>

class ReportStorage::Private : public QObject
{
    Q_OBJECT

public:
    Private()
     : dir(qApp->applicationDirPath())
     ,realTime(true)
    {
        dir.mkdir("reports");
        dir.cd("reports");
    }

    QDir dir;
    bool realTime;
    QPointer<ReportScheduler> scheduler;

public slots:
    void reportAdded(const ReportPtr& report);
    void reportRemoved(const ReportPtr& report);
};

void ReportStorage::Private::reportAdded(const ReportPtr &report)
{
    if ( !realTime )
        return;

    // TODO: Implementation
}

void ReportStorage::Private::reportRemoved(const ReportPtr &report)
{
    if ( !realTime )
        return;

    // TODO: Implementation
}

ReportStorage::ReportStorage(ReportScheduler *scheduler, QObject *parent)
: QObject(parent)
, d(new Private)
{
    d->scheduler = scheduler;

    connect(scheduler, SIGNAL(reportAdded(ReportPtr)), d, SLOT(reportAdded(ReportPtr)));
    connect(scheduler, SIGNAL(reportRemoved(ReportPtr)), d, SLOT(reportRemoved(ReportPtr)));
}

ReportStorage::~ReportStorage()
{
    delete d;
}

void ReportStorage::setRealTime(bool realTime)
{
    d->realTime = realTime;
}

bool ReportStorage::isRealTime() const
{
    return d->realTime;
}

void ReportStorage::loadData()
{

}

void ReportStorage::saveData()
{

}
