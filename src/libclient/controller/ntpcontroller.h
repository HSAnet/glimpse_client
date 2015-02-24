#ifndef NTPCONTROLLER_H
#define NTPCONTROLLER_H

#include "controller.h"

#include <QtEndian>
#include <QDateTime>
#include <QObject>
#include <QUdpSocket>

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
