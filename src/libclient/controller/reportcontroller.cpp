#include "reportcontroller.h"
#include "../settings.h"
#include "../report/reportscheduler.h"
#include "../log/logger.h"
#include "../types.h"

#include "../webrequester.h"
#include "../network/requests/request.h"
#include "../response.h"

#include <QPointer>

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

    // Response interface
    bool fillFromVariant(const QVariantMap &variant) {
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
    LOG_INFO("Reports successfully sent");

    foreach(const ReportPtr& report, post.reports()) {
        scheduler->removeReport(report);
    }
}

void ReportController::Private::onError()
{
    LOG_INFO(QString("Failed to send reports: %1").arg(requester.errorString()));
}

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
