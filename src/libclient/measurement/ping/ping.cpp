#include "ping.h"
#include "../../log/logger.h"

#include <QProcess>

LOGGER(BulkTransportCapacityMA)

Ping::Ping(QObject *parent)
: Measurement(parent)
, stream(&process)
, currentStatus(Ping::Unknown)
{
}

Ping::~Ping()
{
    process.kill();
    process.waitForFinished(500);
}

bool Ping::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<PingDefinition>();
    if ( definition.isNull() ) {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(&process, SIGNAL(started()), this, SLOT(started()));
    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(readyRead()));

    currentStatus = Ping::Unknown;

    return true;
}

bool Ping::start()
{
    // TODO LOG_INFO(QString("Connect to %1:%2").arg(definition->host).arg(definition->port));

    QStringList args;
#ifdef Q_OS_LINUX
    args << "-c" << QString::number(definition->count)
         << "-n" // Don't resolve hostnames
         << "-W" << QString::number(definition->timeout);
#elif defined(Q_OS_MAC)
    args << "-c" << QString::number(definition->count)
         << "-t" << QString::number(definition->timeout * d->count);
#elif defined(Q_OS_WIN)
    args << "-n" << QString::number(definition->count)
         << "-4" // ipv4
         << "-w" << QString::number(definition->timeout * 1000);
#else
#error Platform ping code missing!
#endif

    args << definition->host;

    process.kill();
    process.start("ping", args);

    return true;
}


Measurement::Status Ping::status() const
{
    return currentStatus;
}

void Ping::setStatus(Status status)
{
    if (currentStatus != status) {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Ping::stop()
{
    process.kill();
    return true;
}

ResultPtr Ping::result() const
{
    return ResultPtr();
}

void Ping::started()
{
    pingTime.clear();

    setStatus(Ping::Running);
    emit started();
}

void Ping::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    setStatus(Ping::Finished);
    emit stopped();
}

void Ping::readyRead()
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

        emit ping(time);
    }
}

void Ping::waitForFinished() {
    process.waitForFinished(1000);
}

int Ping::averagePingTime() {
    int time = 0;
    foreach(int t, pingTime)
        time += t;

    return qRound( time / (double)pingTime.size() );
}
