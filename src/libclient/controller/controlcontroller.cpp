#include "controlcontroller.h"
#include "../network/networkmanager.h"
#include "../scheduler/scheduler.h"
#include "../settings.h"

#include <QPointer>

class ControlController::Private : public QObject
{
    Q_OBJECT

public:
    Private(ControlController* q)
    : q(q)
    {
    }

    ControlController* q;

    // Properties
    QPointer<NetworkManager> networkManager;
    QPointer<Scheduler> scheduler;
    QPointer<Settings> settings;
};

ControlController::ControlController(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

ControlController::~ControlController()
{
    delete d;
}

bool ControlController::init(NetworkManager *networkManager, Scheduler *scheduler, Settings *settings)
{
    d->scheduler = scheduler;
    d->settings = settings;
    d->networkManager = networkManager;

    return true;
}

#include "controlcontroller.moc"
