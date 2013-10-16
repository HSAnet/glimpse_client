#include "client.h"
#include "controller/controlcontroller.h"
#include "network/networkmanager.h"
#include "network/requests/registerdevicerequest.h"
#include "network/requests/manualrequest.h"
#include "task/taskexecutor.h"
#include "scheduler/schedulerstorage.h"
#include "report/reportstorage.h"
#include "networkhelper.h"

#include <QUdpSocket>
#include <QBuffer>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>
#include <QTimer>
#include <QHostInfo>
#include <QDataStream>

#if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#else
#ifdef Q_OS_WIN
#include <QJson/Parser>
#else
#include <qjson/parser.h>
#endif
#endif

#include <QDebug>

const QUrl masterUrl = QUrl("https://141.82.49.82:5105");

class Client::Private : public QObject
{
    Q_OBJECT

public:
    Private(Client* q)
    : q(q)
    , status(Client::Unregistered)
    , networkAccessManager(new QNetworkAccessManager(q))
    , schedulerStorage(&scheduler)
    , reportStorage(&reportScheduler)
    {
        executor.setNetworkManager(&networkManager);
        scheduler.setExecutor(&executor);
    }

    Client* q;

    // Properties
    Client::Status status;
    QNetworkAccessManager* networkAccessManager;

    TaskExecutor executor;

    Scheduler scheduler;
    SchedulerStorage schedulerStorage;

    ReportScheduler reportScheduler;
    ReportStorage reportStorage;

    Settings settings;
    NetworkManager networkManager;

    QHostAddress lastLocalIp;

    ControlController controlController;
};

Client::Client(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Client::~Client()
{
    d->schedulerStorage.storeData();
    d->reportStorage.storeData();
    delete d;
}

Client *Client::instance()
{
    static Client* ins = NULL;

    if ( !ins )
        ins = new Client();

    return ins;
}

bool Client::init()
{
    bool ok = false;

    d->settings.init();

    // Initialize storages
    d->schedulerStorage.loadData();
    d->reportStorage.loadData();

    return ok;
}

void Client::setStatus(Client::Status status)
{
    if ( d->status == status )
        return;

    d->status = status;
    emit statusChanged();
}

Client::Status Client::status() const
{
    return d->status;
}

QNetworkAccessManager *Client::networkAccessManager() const
{
    return d->networkAccessManager;
}

Scheduler *Client::scheduler() const
{
    return &d->scheduler;
}

ReportScheduler *Client::reportScheduler() const
{
    return &d->reportScheduler;
}

NetworkManager *Client::networkManager() const
{
    return &d->networkManager;
}

TaskExecutor *Client::taskExecutor() const
{
    return &d->executor;
}

Settings *Client::settings() const
{
    return &d->settings;
}

#include "client.moc"
