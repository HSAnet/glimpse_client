#include "crashcontroller.h"

#include "../network/networkmanager.h"
#include "../network/requests/request.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../storage/storagepaths.h"
#include "../client.h"
#include "../log/logger.h"

#include <QPointer>
#include <QDir>

LOGGER(CrashController);

class UploadRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/crashreports")

public:
    explicit UploadRequest(QObject *parent = 0)
    : Request(parent)
    {
    }

    // Request interface
    QVariant toVariant() const
    {
        QVariantMap map;
        QVariantList list;

        foreach (QString report, m_reports)
        {
            list.append(report);
        }

        map.insert("crashreports", list);
        map.insert("device_id", deviceId());

        return map;
    }

protected:
    QStringList m_reports;
};

class CrashController::Private : public QObject
{
    Q_OBJECT

public:

    Private(CrashController *q)
    : q(q)
    {
        connect(&requester, SIGNAL(statusChanged(WebRequester::Status)), q, SIGNAL(statusChanged()));
        connect(&requester, SIGNAL(finished()), this, SLOT(onFinished()));
        connect(&requester, SIGNAL(error()), this, SLOT(onError()));

        //post.setPath(("/api/v1/crashreport/"));
        //requester.setRequest(&post);
        //requester.setResponse(&response);
    }

    CrashController *q;

    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    QStringList reportFiles;

    WebRequester requester;
    UploadRequest post;
    //ReportResponse response;

public slots:
    void updateResponse();
    void onFinished();
    void onError();
};

void CrashController::Private::updateResponse()
{
    // Set the new url
    QString newUrl = QString("http://%1").arg(Client::instance()->settings()->config()->reportAddress());

    if (requester.url() != newUrl)
    {
        LOG_INFO(QString("Report url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
    }
}

void CrashController::Private::onFinished()
{
    // TODO: cleanup reports
}

void CrashController::Private::onError()
{
    LOG_INFO(QString("Failed to send reports: %1").arg(requester.errorString()));
}

CrashController::CrashController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{
}

CrashController::~CrashController()
{
    delete d;
}

bool CrashController::hasCrashReports() const
{
    return !d->reportFiles.isEmpty();
}

void CrashController::uploadReports()
{
    // TODO: upload crash reports
}

Controller::Status CrashController::status() const
{
    return (Controller::Status)d->requester.status();
}

QString CrashController::errorString() const
{
    return d->requester.errorString();
}

bool CrashController::init(NetworkManager *networkManager, Settings *settings)
{
    d->networkManager = networkManager;
    d->settings = settings;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateResponse()));

    // fetch all crash report files
    d->reportFiles = StoragePaths().crashDumpDirectory().entryList(
            QStringList("*.dmp"), QDir::Files);

    return true;
}

#include "crashcontroller.moc"
