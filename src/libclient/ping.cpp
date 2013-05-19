#include "ping.h"

#include <QProcess>
#include <QTextStream>
#include <QRegExp>

class Ping::Private : public QObject
{
    Q_OBJECT

public:
    Private(Ping* q)
    : q(q)
    , status(Ping::NotRunning)
    , amount(10)
    , timeout(1)
    , stream(&process)
    {
        connect(&process, SIGNAL(started()), this, SLOT(started()));
        connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
        connect(&process, SIGNAL(readyRead()), this, SLOT(readyRead()));
    }

    Ping* q;

    // Properties
    Ping::Status status;
    QString host;
    int amount;
    int timeout;

    QList<int> pingTime;

    QProcess process;
    QTextStream stream;

    // Functions
    void setStatus(Ping::Status status);

public slots:
    void started();
    void finished(int exitCode, QProcess::ExitStatus exitStatus);
    void readyRead();
};

void Ping::Private::setStatus(Ping::Status status)
{
    if (this->status != status) {
        this->status = status;
        emit q->statusChanged(status);
    }
}

void Ping::Private::started()
{
    pingTime.clear();

    setStatus(Ping::Running);
    emit q->started();
}

void Ping::Private::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    setStatus(Ping::NotRunning);
    emit q->stopped();
}

void Ping::Private::readyRead()
{
    QRegExp re;
#if defined(Q_OS_WIN)
    // Antwort von 193.99.144.80: Bytes=32 Zeit=32ms TTL=245
    re.setPattern("=(\\d+)ms");
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    // 64 bytes from 193.99.144.80: icmp_seq=0 ttl=245 time=32.031 ms
    re.setPattern("time=(\\d+).*ms");
#else
#error Platform code not implemented
#endif

    for(QString line = stream.readLine(); !line.isNull(); line = stream.readLine()) {
        if ( re.indexIn(line) == -1 )
            continue;

        int time = re.cap(1).toInt();
        pingTime.append(time);

        emit q->ping(time);
    }
}

Ping::Ping(QObject *parent)
: QObject(parent)
, d(new Private(this))
{
}

Ping::~Ping()
{
    d->process.kill();
    d->process.waitForFinished(500);
    delete d;
}

Ping::Status Ping::status() const
{
    return d->status;
}

Ping::ExitStatus Ping::exitStatus() const
{
    return (Ping::ExitStatus)d->process.exitStatus();
}

void Ping::setHost(const QString &host)
{
    if (d->host != host) {
        d->host = host;
        emit hostChanged(host);
    }
}

QString Ping::host() const
{
    return d->host;
}

void Ping::setAmount(int amount)
{
    if (d->amount != amount) {
        d->amount = amount;
        emit amountChanged(amount);
    }
}

int Ping::amount() const
{
    return d->amount;
}

void Ping::setTimeout(int timeout)
{
    if (d->timeout != timeout) {
        d->timeout = timeout;
        emit timeoutChanged(timeout);
    }
}

int Ping::timeout() const
{
    return d->timeout;
}

QList<int> Ping::pingTime() const
{
    return d->pingTime;
}

int Ping::averagePingTime() const
{
    int time = 0;
    foreach(int t, d->pingTime)
        time += t;

    return qRound( time / (double)d->pingTime.size() );
}

void Ping::start()
{
    QStringList args;
#ifdef Q_OS_LINUX
    args << "-c" << QString::number(d->amount)
         << "-n" // Don't resolve hostnames
#ifndef Q_OS_ANDROID
         << "-W" << QString::number(d->timeout);
#else
         /*<< "-w" << QString::number(d->timeout)*/;
#endif
#elif defined(Q_OS_MAC)
    args << "-c" << QString::number(d->amount)
         << "-t" << QString::number(d->timeout);
#elif defined(Q_OS_WIN)
    args << "-n" << QString::number(d->amount)
         << "-4" // ipv4
         << "-w" << QString::number(d->timeout * 1000);
#else
#error Platform ping code missing!
#endif

    args << d->host;

    d->process.start("ping", args);
}

void Ping::stop()
{
    d->process.kill();
}

#include "ping.moc"
