#include "reportcontroller.h"
#include "../settings.h"
#include "../report/reportscheduler.h"
#include "../log/logger.h"

#include <QPointer>

LOGGER(ReportController);

class ReportController::Private
{
public:
    Private(ReportController* q)
    : q(q)
    {
    }

    ReportController* q;

    // Properties
    QPointer<ReportScheduler> scheduler;
    QPointer<Settings> settings;
};

ReportController::ReportController(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

ReportController::~ReportController()
{
    delete d;
}

bool ReportController::init(ReportScheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;
    return true;
}

void ReportController::sendReports()
{
    LOG_INFO("Sending reports");
}
