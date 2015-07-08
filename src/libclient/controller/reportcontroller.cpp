#include "reportcontroller.h"
#include "../settings.h"
#include "../report/reportscheduler.h"
#include "../log/logger.h"
#include "../types.h"

#include "../webrequester.h"
#include "../network/requests/request.h"
#include "../network/requests/postrequest.h"
#include "../network/responses/reportresponse.h"
#include "../timing/periodictiming.h"
#include "../client.h"
#include "../timing/timer.h"
#include "../storage/storagepaths.h"

#include <QPointer>
#include <QStringList>
#include <QTimer>
#include <QDir>
#include <QRegExp>
#include <QDateTime>

LOGGER(ReportController);


class ReportPost : public Request
{
    Q_OBJECT

    Q_CLASSINFO("http_request_method", "post")
    Q_CLASSINFO("authentication_method", "none")

public:
    ReportPost(QObject *parent = 0)
    : Request(parent)
    {
    }

    void setReport(const Report &report)
    {
        m_report = report;
    }

    Report report() const
    {
        return m_report;
    }

    // Request interface
    QVariant toVariant() const
    {
        return m_report.toVariant();
    }

protected:
    Report m_report;
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

        post.setPath(("/register/result"));
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
    void rotate();
};

void ReportController::Private::updateTimer()
{
    // Set the new url
    QString newUrl = QString("http://%1").arg(settings->config()->supervisorAddress());

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
    LOG_DEBUG("Results successfully inserted");
}

void ReportController::Private::onError()
{
    LOG_ERROR(QString("Failed to send reports: %1").arg(requester.errorString()));
}

void ReportController::Private::onReportAdded()
{
    q->sendReports();
}

void ReportController::Private::onTimingChanged()
{
    LOG_DEBUG(QString("Report schedule changed, type: %1, nextRun in %2 seconds.")
              .arg(timer.timing()->type())
              .arg(timer.timing()->timeLeft() / 1000));
}

void ReportController::Private::rotate()
{
    // <task-id>_yyyy-MM-dd.json
    QRegExp regex("^\\d+_(\\d{4}-\\d{2}-\\d{2}).json$");
    QDir dir(StoragePaths().resultDirectory());
    QDate oldest = QDateTime::currentDateTime().addDays(-static_cast<qint64>(settings->backlog())).date();

    foreach (const QString &file, dir.entryList(QDir::Files))
    {
        if (regex.exactMatch(file))
        {
            if (QDateTime::fromString(regex.cap(1), "yyyy-MM-dd").date() < oldest)
            {
                dir.remove(file);
            }
        }
    }
}

ReportController::ReportController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

ReportController::~ReportController()
{
    d->rotate();
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
    connect(d->scheduler, SIGNAL(reportAdded(Report)), d, SLOT(rotate()));
    connect(d->scheduler, SIGNAL(reportModified(Report)), d, SLOT(onReportAdded()));
    connect(d->scheduler, SIGNAL(reportModified(Report)), d, SLOT(rotate()));

    d->updateTimer();

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

    foreach (const Report &report, reports)
    {
        if (!report.timing()->reset())
        {
            d->post.setReport(report);
            d->requester.start();
            d->scheduler->removeReport(report);
        }
    }
}

#include "reportcontroller.moc"
