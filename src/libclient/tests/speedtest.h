#ifndef SPEEDTEST_H
#define SPEEDTEST_H

#include "test.h"

class SpeedTest : public AbstractTest
{
    Q_OBJECT
    Q_PROPERTY(qreal maximumDownloadSpeed READ maximumDownloadSpeed NOTIFY maximumDownloadSpeedChanged)
    Q_PROPERTY(qreal averageDownloadSpeed READ averageDownloadSpeed NOTIFY averageDownloadSpeedChanged)
    Q_PROPERTY(qreal progress READ progress NOTIFY progressChanged)

public:
    explicit SpeedTest(QObject *parent = 0);
    ~SpeedTest();

    qreal maximumDownloadSpeed() const;
    qreal averageDownloadSpeed() const;
    qreal progress() const;

    // AbstractTest overrides
    QString name() const;
    bool isMaster() const;

    bool initialize(const PeerList& peers, bool master, QUdpSocket* socket);
    void uninitialize();

    bool isFinished() const;

    void processDatagram(const QByteArray& datagram, const QHostAddress& host, quint16 port);

    QVariant result() const;

public slots:
    bool start();
    bool stop();

    QVariant data(int role) const;

signals:
    void progressChanged(qreal progress);
    void maximumDownloadSpeedChanged(qreal maximumDownloadSpeed);
    void averageDownloadSpeedChanged(qreal averageDownloadSpeed);

protected:
    class Private;
    friend class Private;
    Private* d;
};

#endif // SPEEDTEST_H
