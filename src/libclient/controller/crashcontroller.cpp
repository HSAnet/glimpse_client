#include "crashcontroller.h"

#include "../network/networkmanager.h"
#include "../settings.h"
#include "../webrequester.h"

#include <QPointer>
#include <QDir>

class UploadRequest : public Request
{
    Q_OBJECT
    Q_CLASSINFO("path", "/crashreports")

public:
    explicit UploadRequest(QObject *parent = 0)
    : Request(parent)
    {
    }
};

class CrashController::Private
{
public:
    QPointer<NetworkManager> networkManager;
    QPointer<Settings> settings;

    QStringList reportFiles;

    WebRequester requester;
};

CrashController::CrashController(QObject *parent)
: Controller(parent)
, d(new Private)
{
}

CrashController::~CrashController()
{
    delete d;
}

bool CrashController::hasCrashReports() const
{
    return d->reportFiles.isEmpty();
}

void CrashController::uploadReports()
{
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
    return true;
}
