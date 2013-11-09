#include "reportcontroller.h"
#include "../settings.h"
#include "../report/reportscheduler.h"
#include "../log/logger.h"
#include "../types.h"

#include "../webrequester.h"
#include "../network/requests/request.h"
#include "../response.h"

#include <QPointer>
#include <QStringList>

LOGGER(ReportController);


class ReportPost : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/send_results")

public:
    ReportPost(QObject* parent = 0)
    : Request(parent)
    {
    }

    void setReports(const ReportList& reports) {
        m_reports = reports;
    }

    ReportList reports() const {
        return m_reports;
    }

    // Request interface
    QVariant toVariant() const {
        QVariantMap map;
        map.insert("session_id", sessionId());
        map.insert("reports", ptrListToVariant(m_reports));
        return map;
    }

protected:
    ReportList m_reports;
};


class ReportResponse : public Response
{
    Q_OBJECT

public:
    ReportResponse(QObject* parent = 0)
    : Response(parent)
    {
    }

    QStringList taskIds;

    // Response interface
    bool fillFromVariant(const QVariantMap &variant) {
        this->taskIds.clear();

        QVariantList taskIds = variant.value("successful_task_ids").toList();
        foreach(const QVariant& id, taskIds)
            this->taskIds.append(id.toString());

        return true;
    }

    void finished() {
    }
};


class ReportController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ReportController* q)
    : q(q)
    {
        connect(&requester, SIGNAL(statusChanged(Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));

        requester.setRequest(&post);
        requester.setResponse(&response);
    }

    ReportController* q;

    // Properties
    QPointer<ReportScheduler> scheduler;
    QPointer<Settings> settings;

    WebRequester requester;
    ReportPost post;
    ReportResponse response;

public slots:
    void onFinished();
    void onError();
};

void ReportController::Private::onFinished()
{
    QStringList taskIds = response.taskIds;
    LOG_INFO(QString("%1 Reports successfully sent").arg(taskIds.size()));

    foreach(const QString& taskId, taskIds) {
        ReportPtr report = scheduler->reportByTaskId(taskId);
        if ( report.isNull() ) {
            LOG_ERROR(QString("No task with id %1 found.").arg(taskId));
        } else {
            scheduler->removeReport(report);
        }
    }
}

void ReportController::Private::onError()
{
    LOG_INFO(QString("Failed to send reports: %1").arg(requester.errorString()));
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
    return true;
}

QString ReportController::errorString() const
{
    return d->requester.errorString();
}

void ReportController::sendReports()
{
    ReportList reports = d->scheduler->reports();

    if ( reports.isEmpty() ) {
        LOG_INFO("No reports to send");
        return;
    }

    LOG_INFO(QString("Sending %1 reports").arg(reports.size()));

    d->post.setReports(reports);
    d->requester.start();
}

#include "reportcontroller.moc"
