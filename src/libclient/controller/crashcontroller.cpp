#include "crashcontroller.h"

#include "../network/networkmanager.h"
#include "../network/requests/uploadrequest.h"
#include "../settings.h"
#include "../webrequester.h"
#include "../storage/storagepaths.h"
#include "../client.h"
#include "../log/logger.h"

#include <QPointer>
#include <QDir>

#ifdef HAVE_BREAKPAD
#include <crashhandler.h>
#endif // HAVE_BREAKPAD

LOGGER(CrashController);

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

        request.setPath(("/api/v1/crashreport/"));
        requester.setRequest(&request);
    }

    CrashController *q;

#ifdef HAVE_BREAKPAD
    CrashHandler crashHandler;
#endif // HAVE_BREAKPAD

    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    QStringList reportFiles;

    WebRequester requester;
    UploadRequest request;

public slots:
    void updateResponse();
    void onFinished();
    void onError();
};

void CrashController::Private::updateResponse()
{
    // Set the new url
    QString newUrl = QString("https://%1").arg(Client::instance()->settings()->config()->reportAddress());

    if (requester.url() != newUrl)
    {
        LOG_INFO(QString("Crash url set to %1").arg(newUrl));
        requester.setUrl(newUrl);
    }
}

void CrashController::Private::onFinished()
{
    // remove from filesystem
    foreach (const QString &fileName, reportFiles)
    {
        QFile file(StoragePaths().crashDumpDirectory().absoluteFilePath(fileName));
        file.remove();
    }

    // clear list
    reportFiles.clear();
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
    d->request.clearResources();

    foreach (const QString &fileName, d->reportFiles)
    {
        QFile file(StoragePaths().crashDumpDirectory().absoluteFilePath(fileName));

        if (!file.open(QIODevice::ReadOnly))
        {
            LOG_DEBUG(QString("Error opening %1: %2").arg(StoragePaths().crashDumpDirectory().absoluteFilePath(fileName)).arg(file.errorString()));
            continue;
        }

        QByteArray data = file.readAll();
        // compress data, remove the first four bytes (thich is the array length which does not belong there), convert to base64
        d->request.addResource(qCompress(data).remove(0,4).toBase64());
        file.close();
    }

    d->requester.start();
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
    QDir crashdumpDir = StoragePaths().crashDumpDirectory();

    if (!crashdumpDir.exists())
    {
        if (!QDir::root().mkpath(crashdumpDir.absolutePath()))
        {
            LOG_ERROR(QString("Failed to create crashdump directory: %1").arg(crashdumpDir.absolutePath()));
            return false;
        }
        else
        {
            LOG_INFO("Crashdump directory created");
        }
    }

#ifdef HAVE_BREAKPAD
    d->crashHandler.init(crashdumpDir.absolutePath());
#endif // HAVE_BREAKPAD

    d->networkManager = networkManager;
    d->settings = settings;

    connect(settings->config(), SIGNAL(responseChanged()), d, SLOT(updateResponse()));

    // fetch all crash report files
    d->reportFiles = StoragePaths().crashDumpDirectory().entryList(
            QStringList("*.dmp"), QDir::Files);

    d->updateResponse();

    return true;
}

#include "crashcontroller.moc"
