#include "ping.h"
#include "../../log/logger.h"

#include <QProcess>
#include <QtMath>
#include <limits.h>

LOGGER(Ping);

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

    if (definition.isNull())
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(&process, SIGNAL(started()), this, SLOT(started()));
    connect(&process, SIGNAL(finished(int, QProcess::ExitStatus)), this, SLOT(finished(int, QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(readyRead()));

    currentStatus = Ping::Unknown;

    return true;
}

bool Ping::start()
{
    setStartDateTime(QDateTime::currentDateTime());

    QStringList args;

#ifdef Q_OS_LINUX
    args << "-c" << QString::number(definition->count)
         << "-n" // Don't resolve hostnames
         << "-W" << QString::number((float)definition->timeout / 1000)
         << "-i" << QString::number((float)definition->interval / 1000);
#elif defined(Q_OS_MAC)
    args << "-c" << QString::number(definition->count)
         << "-W" << QString::number(definition->timeout)
         << "-i" << QString::number((float)definition->interval / 1000);
#elif defined(Q_OS_WIN)
    args << "-n" << QString::number(definition->count)
         << "-4" // ipv4
         << "-w" << QString::number(definition->timeout);
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
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Ping::stop()
{
    process.kill();
    return true;
}

Result Ping::result() const
{
    QVariantMap res;
    QVariantList roundTripMs;
    float variance = 0.0;

    float avg = averagePingTime();

    // get max and min
    QList<float>::const_iterator it = std::max_element(pingTime.begin(), pingTime.end());
    float max = *it;
    it = std::min_element(pingTime.begin(), pingTime.end());
    float min = *it;

    foreach (float val, pingTime)
    {
        roundTripMs << QString::number(val, 'f', 3);
        variance += (val-avg) * (val-avg);
    }

    variance = variance / pingTime.length();

    res.insert("round_trip_ms", roundTripMs);
    res.insert("round_trip_avg", avg);
    res.insert("round_trip_min", min);
    res.insert("round_trip_max", max);
    res.insert("round_trip_stddev", qSqrt(qAbs(variance)));

    return Result(startDateTime(), endDateTime(), res, QVariant());
}

void Ping::started()
{
    pingTime.clear();

    setStatus(Ping::Running);
}

void Ping::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    setStatus(Ping::Finished);
    setEndDateTime(QDateTime::currentDateTime());
    emit Measurement::finished();
}

void Ping::readyRead()
{
    QRegExp re;
#if defined(Q_OS_WIN)
    // Antwort von 193.99.144.80: Bytes=32 Zeit=32ms TTL=245
    re.setPattern("=(\\d+)ms");
#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
    // 64 bytes from 193.99.144.80: icmp_seq=0 ttl=245 time=32.031 ms
    re.setPattern("time=(\\d+.*)ms");
#else
#error Platform code not implemented
#endif

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
    {
        if (re.indexIn(line) == -1)
        {
            continue;
        }

        float time = re.cap(1).toFloat();
        pingTime.append(time);

        emit ping(time);
    }
}

void Ping::waitForFinished()
{
    process.waitForFinished(1000);
}

float Ping::averagePingTime() const
{
    float time = 0;

    foreach (float t, pingTime)
    {
        time += t;
    }

    return time / pingTime.size();
}
