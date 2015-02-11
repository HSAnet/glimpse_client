#include "reportcontroller.h"
#include "../settings.h"
#include "../report/reportscheduler.h"
#include "../log/logger.h"
#include "../types.h"

#include "../webrequester.h"
#include "../network/requests/request.h"
#include "../network/responses/reportresponse.h"
#include "../timing/periodictiming.h"
#include "../client.h"
#include "../timing/timer.h"

#include <QPointer>
#include <QStringList>
#include <QTimer>

LOGGER(ReportController);


class ReportPost : public Request
{
    Q_OBJECT

    Q_CLASSINFO("http_request_method", "post")
    Q_CLASSINFO("authentication_method", "apikey")

public:
    ReportPost(QObject *parent = 0)
    : Request(parent)
    {
    }

    void setReports(const ReportList &reports)
    {
        m_reports = reports;
    }

    ReportList reports() const
    {
        return m_reports;
    }

    // Request interface
    QVariant toVariant() const
    {
        QVariantMap map;
        QVariantList list;

        foreach (Report report, m_reports)
        {
            list.append(report.toVariant());
        }

        map.insert("reports", list);
        map.insert("device_id", deviceId());

        return map;
    }

protected:
    ReportList m_reports;
};


class ReportController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ReportController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(sendReports()));
        connect(&timer, SIGNAL(timingChanged()), this, SLOT(onTimingChanged()));
        connect(&requester, SIGNAL(statusChanged(WebRequester::Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));

        post.setPath(("/api/v1/report/"));
        requester.setRequest(&post);
        requester.setResponse(&response);
    }

    ReportController *q;

    // Properties
    QPointer<ReportScheduler> scheduler;
    QPointer<Settings> settings;

    WebRequester requester;
    ReportPost post;
    ReportResponse response;

    Timer timer;

    bool isImmediate;

public slots:
    void updateTimer();
    void onFinished();
    void onError();
    void onReportAdded();
    void onTimingChanged();
};

void ReportController::Private::updateTimer()
{
    // Set the new url
    QString newUrl = QString("https://%1").arg(settings->config()->reportAddress());

    if (requester.url() != newUrl)
    {
        LOG_DEBUG(QString("Report url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
    }

    TimingPtr timing = settings->config()->reportTiming();

    if (timing.isNull())
    {
        timer.stop();
        return;
    }

    isImmediate = (timing->type() == "immediate");

    if (!isImmediate)
    {
        timer.setTiming(timing);
        timer.start();
    }
}

void ReportController::Private::onFinished()
{
    QList<TaskId> taskIds = response.taskIds;
    LOG_DEBUG(QString("%1 Results successfully inserted").arg(taskIds.size()));

    foreach (const TaskId &taskId, taskIds)
    {
        Report report = scheduler->reportByTaskId(taskId);

        if (report.isNull())
        {
            LOG_WARNING(QString("No task with id %1 found.").arg(taskId.toInt()));
        }
        else
        {
            scheduler->removeReport(report);
        }
    }
}

void ReportController::Private::onError()
{
    LOG_ERROR(QString("Failed to send reports: %1").arg(requester.errorString()));
}

void ReportController::Private::onReportAdded()
{
    if (isImmediate)
    {
        q->sendReports();
    }
}

void ReportController::Private::onTimingChanged()
{
    LOG_DEBUG(QString("Report schedule changed, type: %1, nextRun in %2 seconds.")
              .arg(timer.timing()->type())
              .arg(timer.timing()->timeLeft() / 1000));
}

ReportController::ReportController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

ReportController::~ReportController()
{
    delete d;
}

ReportController::Status ReportController::status() const
{
    return (Status)d->requester.status();
}

bool ReportController::init(ReportScheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateTimer()));
    connect(d->scheduler, SIGNAL(reportAdded(Report)), d, SLOT(onReportAdded()));
    connect(d->scheduler, SIGNAL(reportModified(Report)), d, SLOT(onReportAdded()));

    return true;
}

QString ReportController::errorString() const
{
    return d->requester.errorString();
}

void ReportController::sendReports()
{
    ReportList reports = d->scheduler->reports();

    if (reports.isEmpty())
    {
        LOG_DEBUG("No reports to send");
        return;
    }

    LOG_DEBUG(QString("Sending %1 reports").arg(reports.size()));

    d->post.setReports(reports);
    d->requester.start();
}

#include "reportcontroller.moc"
