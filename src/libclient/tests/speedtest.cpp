#include "speedtest.h"
#include "../client.h"

#include <QUrl>
#include <QNetworkReply>
#include <QPointer>
#include <QTime>

class SpeedTest::Private : public QObject
{
    Q_OBJECT

public:
    Private(SpeedTest* q)
    : q(q)
    , isRunning(false)
    , downloadUrl("http://www.avm.de/de/Service/FRITZ_Tools/zack/img2500.jpg")
    , maximumDownloadSpeed(0.0)
    , averageDownloadSpeed(0.0)
    , progress(0.0)
    {
    }

    SpeedTest* q;

    // Properties
    bool isRunning;
    QUrl downloadUrl;

    QPointer<QNetworkReply> reply;
    QTime time;

    qreal maximumDownloadSpeed;
    qreal averageDownloadSpeed;
    qreal progress;

    // Functions
public slots:
    void downloadProgress(qint64 bytesReceived, qint64 bytesTotal);
    void readyRead();
    void finished();
};

void SpeedTest::Private::downloadProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    // Start the timer on the first call
    if ( time.isNull() )
    {
        time.restart();
    }
    else
    {
        qint64 packetCount = bytesReceived / 1452;
        qint64 realBytesReceived = packetCount * 1500; // TODO: Check if this is really the packet size

        averageDownloadSpeed = realBytesReceived*1000 / time.elapsed();
        emit q->averageDownloadSpeedChanged(averageDownloadSpeed);

        if ( averageDownloadSpeed > maximumDownloadSpeed )
        {
            maximumDownloadSpeed = averageDownloadSpeed;
            emit q->maximumDownloadSpeedChanged(maximumDownloadSpeed);
        }
    }

    progress = (qreal)bytesReceived / (qreal)bytesTotal;
    emit q->progressChanged(progress);
}

void SpeedTest::Private::readyRead()
{
    // Clear buffers, so we read all to nowhere
    QByteArray data = reply->readAll();
    //qDebug() << data.size();
}

void SpeedTest::Private::finished()
{
    isRunning = false;
    reply->deleteLater();

    emit q->finished();
    emit q->stopped();
}

SpeedTest::SpeedTest(QObject *parent)
: AbstractTest(parent)
, d(new Private(this))
{
}

SpeedTest::~SpeedTest()
{
    delete d;
}

qreal SpeedTest::maximumDownloadSpeed() const
{
    return d->maximumDownloadSpeed;
}

qreal SpeedTest::averageDownloadSpeed() const
{
    return d->averageDownloadSpeed;
}

qreal SpeedTest::progress() const
{
    return d->progress;
}

QString SpeedTest::name() const
{
    return "speedtest";
}

bool SpeedTest::isMaster() const
{
    return true;
}

bool SpeedTest::initialize(const PeerList &peers, bool master, QUdpSocket *socket)
{
    Q_UNUSED(peers);
    Q_UNUSED(master);
    Q_UNUSED(socket);

    return true;
}

void SpeedTest::uninitialize()
{
}

bool SpeedTest::isFinished() const
{
    return !d->isRunning;
}

void SpeedTest::processDatagram(const QByteArray &datagram, const QHostAddress &host, quint16 port)
{
    // I do not wand datagrams!
    Q_UNUSED(datagram);
    Q_UNUSED(host);
    Q_UNUSED(port);
}

QVariant SpeedTest::result() const
{
    QVariantMap values;
    values.insert("average-download-speed", d->averageDownloadSpeed);
    values.insert("maximum-download-speed", d->maximumDownloadSpeed);
    values.insert("download-progress", d->progress);
    // FIXME: Don't use hardcoded values
    values.insert("file-size", 12*1024*1024);
    values.insert("packet-count", 12*1024*1024 / 1452);
    return values;
}

bool SpeedTest::start()
{
    if ( d->isRunning )
    {
        return false;
    }

    QNetworkRequest request(d->downloadUrl);

    d->isRunning = true;
    d->reply = Client::instance()->networkAccessManager()->get(request);
    d->reply->setReadBufferSize(0);
    connect(d->reply.data(), SIGNAL(downloadProgress(qint64,qint64)), d, SLOT(downloadProgress(qint64,qint64)));
    connect(d->reply.data(), SIGNAL(readyRead()), d, SLOT(readyRead()));
    connect(d->reply.data(), SIGNAL(finished()), d, SLOT(finished()));

    emit started();

    return true;
}

bool SpeedTest::stop()
{
    if ( d->isRunning )
    {
        d->reply->abort();
        emit stopped();
        return true;
    }

    return false;
}

QVariant SpeedTest::data(int role) const
{
    Q_UNUSED(role);
    return QVariant();
}

#include "speedtest.moc"
