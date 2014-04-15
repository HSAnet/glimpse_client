#include "dnslookup.h"
#include "../../log/logger.h"

#include <QProcess>

LOGGER(Dnslookup);

Dnslookup::Dnslookup(QObject *parent)
: Measurement(parent)
, stream(&process)
, currentStatus(Dnslookup::Unknown)
{
}

Dnslookup::~Dnslookup()
{
    process.kill();
    process.waitForFinished(500);
}

bool Dnslookup::prepare(NetworkManager *networkManager, const MeasurementDefinitionPtr &measurementDefinition)
{
    Q_UNUSED(networkManager);
    definition = measurementDefinition.dynamicCast<DnslookupDefinition>();
    if ( definition.isNull() )
    {
        LOG_WARNING("Definition is empty");
        return false;
    }

    connect(&process, SIGNAL(started()), this, SLOT(started()));
    connect(&process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(finished(int,QProcess::ExitStatus)));
    connect(&process, SIGNAL(readyRead()), this, SLOT(readyRead()));

    currentStatus = Dnslookup::Unknown;

    return true;
}

bool Dnslookup::start()
{
    QStringList args;
//#ifdef Q_OS_LINUX
//    args << "-c" << QString::number(definition->count)
//         << "-n" // Don't resolve hostnames
//         << "-W" << QString::number((float)definition->timeout / 1000)
//         << "-i" << QString::number((float)definition->interval / 1000);
//#elif defined(Q_OS_MAC)
//    args << "-c" << QString::number(definition->count)
//         << "-W" << QString::number(definition->timeout)
//         << "-i" << QString::number((float)definition->interval / 1000);
//#elif defined(Q_OS_WIN)
//    args << "-n" << QString::number(definition->count)
//         << "-4" // ipv4
//         << "-w" << QString::number(definition->timeout);
//#else
//#error Platform dnslookup code missing!
//#endif

//    args << definition->host;
    args << "-q=mx ";
    args << definition->host;
    process.kill();
    process.start("nslookup", args); //QDNSLookup

    return true;
}


Measurement::Status Dnslookup::status() const
{
    return currentStatus;
}

void Dnslookup::setStatus(Status status)
{
    if (currentStatus != status)
    {
        currentStatus = status;
        emit statusChanged(status);
    }
}

bool Dnslookup::stop()
{
    process.kill();
    return true;
}

ResultPtr Dnslookup::result() const
{
    QVariantList res;
    foreach(QString val, dnslookupOutput)
    {
        res << val;
    }

    return ResultPtr(new Result(QDateTime::currentDateTime(), res, QVariant()));
}

void Dnslookup::started()
{
    dnslookupTime.clear();
    dnslookupOutput.clear();

    setStatus(Dnslookup::Running);
}

void Dnslookup::finished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);

    setStatus(Dnslookup::Finished);
    emit Measurement::finished();
}

void Dnslookup::readyRead()
{
//    QRegExp re;
//#if defined(Q_OS_WIN)
//    // Antwort von 193.99.144.80: Bytes=32 Zeit=32ms TTL=245
//    re.setPattern("=(\\d+)ms");
//#elif defined(Q_OS_MAC) || defined(Q_OS_LINUX)
//    // 64 bytes from 193.99.144.80: icmp_seq=0 ttl=245 time=32.031 ms
//    re.setPattern("time=(\\d+.*)ms");
//#else
//#error Platform code not implemented
//#endif

//    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
//    {
//        if ( re.indexIn(line) == -1 )
//        {
//            continue;
//        }

//        float time = re.cap(1).toFloat();
//        dnslookupTime.append(time);

//        emit dnslookup(time);
//    }

    for (QString line = stream.readLine(); !line.isNull(); line = stream.readLine())
    {
        dnslookupOutput.append(line);
    }
}

void Dnslookup::waitForFinished()
{
    process.waitForFinished(1000);
}

float Dnslookup::averageDnslookupTime()
{
    float time = 0;
    foreach(float t, dnslookupTime)
    {
        time += t;
    }

    return time / dnslookupTime.size();
}
