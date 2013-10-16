#include "alivecontroller.h"
#include "../settings.h"
#include "../networkhelper.h"

#include <QPointer>
#include <QAbstractSocket>
#include <QJsonDocument>
#include <QStringList>
#include <QTimer>
#include <QDebug>

class AliveController::Private : public QObject
{
    Q_OBJECT

public:
    Private()
    {
        connect(&timer, SIGNAL(timeout()), this, SLOT(timeout()));
    }

    QTimer timer;
    QPointer<QAbstractSocket> socket;
    QPointer<Settings> settings;

    void updateTimer();

public slots:
    void timeout();
};

void AliveController::Private::updateTimer()
{
    int interval = settings->config()->keepaliveSchedule()->interval();
    timer.setInterval(interval);
    timer.start();
}

void AliveController::Private::timeout()
{
    QString sessionId = settings->sessionId();
    if (sessionId.isEmpty())
        return;

    QStringList srcIp;
    srcIp.append( NetworkHelper::localIpAddress().toString() );

    QVariantMap map;
    map.insert("type", "keepalive");
    map.insert("session_id", sessionId);
    map.insert("src_ip", srcIp);
    map.insert("src_port", 1337);

    QByteArray data = QJsonDocument::fromVariant(map).toJson();
    socket->write(data);

    qDebug() << "Alive packet sent";
}

AliveController::AliveController(QObject *parent)
: QObject(parent)
, d(new Private)
{
}

AliveController::~AliveController()
{
    delete d;
}

bool AliveController::init(Settings *settings)
{
    d->settings = settings;
    d->updateTimer();
    return true;
}

void AliveController::setSocket(QAbstractSocket *socket)
{
    d->socket = socket;
}

QAbstractSocket* AliveController::socket() const
{
    return d->socket;
}

void AliveController::setRunning(bool running)
{
    if (isRunning() == running)
        return;

    if ( !running )
        d->timer.stop();
    else
        d->updateTimer();
}

bool AliveController::isRunning() const
{
    return d->timer.isActive();
}

#include "alivecontroller.moc"
