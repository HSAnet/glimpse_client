#include <QTimer>
#include <QHostInfo>

#include "ntpcontroller.h"
#include "../log/logger.h"
#include "../timing/ntp.h"

LOGGER(NtpController);

static const int interval = 3600000;

class NtpController::Private : public QObject
{
    Q_OBJECT

public:
    Private(NtpController *q)
    : q(q)
    {
        connect(&timer, SIGNAL(timeout()), q, SLOT(update()));
        timer.start(interval);
    }

    NtpController *q;
    Ntp ntp;
    QTimer timer;
};

NtpController::NtpController(QObject *parent)
: Controller(parent)
, d(new Private(this))
{

}

NtpController::~NtpController()
{
    delete d;
}

void NtpController::update()
{
    QHostInfo hostInfo = QHostInfo::fromName("ptbtime1.ptb.de");

    if (!hostInfo.addresses().isEmpty())
    {
        QHostAddress ntpServer = hostInfo.addresses().first();
        d->ntp.sync(ntpServer);
    }
    else
    {
        LOG_WARNING("could not resolve ntp server");
    }
}

Controller::Status NtpController::status() const
{
    return Controller::Unknown;
}

QString NtpController::errorString() const
{
    return "";
}

bool NtpController::init()
{
    update();

    return true;
}

quint64 NtpController::offset() const
{
    return d->ntp.offset();
}

#include "ntpcontroller.moc"
