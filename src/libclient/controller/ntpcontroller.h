#ifndef NTPCONTROLLER_H
#define NTPCONTROLLER_H

#include "controller.h"

#include <QtEndian>
#include <QDateTime>
#include <QObject>
#include <QUdpSocket>

struct NtpTimestamp
{
    quint32 seconds;
    quint32 fraction;

    static QDateTime toDateTime(const NtpTimestamp &timestamp)
    {
        quint32 seconds = qFromBigEndian(timestamp.seconds);
        quint32 fraction = qFromBigEndian(timestamp.fraction);
        qint64 ms = seconds * 1000ll + fraction * 1000ll / 0x100000000ll;

        return QDateTime::fromMSecsSinceEpoch(ms - 2208988800000ll);
    }

    static NtpTimestamp fromDateTime(const QDateTime &timestamp)
    {
        NtpTimestamp result;
        qint64 ms = timestamp.toMSecsSinceEpoch() + 2208988800000ll;
        quint32 seconds = ms / 1000;
        quint32 fraction = 0x100000000ll * (ms % 1000) / 1000;

        result.seconds = qToBigEndian(seconds);
        result.fraction = qToBigEndian(fraction);

        return result;
    }
};

struct NtpPacket
{
    struct NtpFlags
    {
        quint8 mode: 3;
        quint8 versionNumber: 3;
        quint8 leapIndicator: 2;
    } flags;

    quint8 peerClockStratum;
    quint8 peerPollingInterval;
    quint8 peerClockPrecision;

    quint32 rootDelay;
    quint32 rootDispersion;
    quint32 referenceId;

    NtpTimestamp referenceTimestamp;
    NtpTimestamp originTimestamp;
    NtpTimestamp receiveTimestamp;
    NtpTimestamp transmitTimestamp;
};

class CLIENT_API NtpController : public Controller
{
    Q_OBJECT

public:
    explicit NtpController(QObject *parent = 0);
    ~NtpController();

    // Controller interface
    Status status() const;
    QString errorString() const;

    bool init();
    bool sync(QHostAddress &host);
    QDateTime localTime() const;
    QDateTime networkTime() const;
    QDateTime currentDateTime() const;
    quint64 offset() const;

signals:
    void error(QString message);

private:
    QUdpSocket *m_socket;
    QDateTime m_localTime;
    QDateTime m_networkTime;

private slots:
    void readResponse();

public slots:
    void update();

protected:
    class Private;
    Private *d;
};

#endif // NTPCONTROLLER_H
