#ifndef NTPCONTROLLER_H
#define NTPCONTROLLER_H

#include "controller.h"

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
    quint64 offset() const;

public slots:
    void update();

protected:
    class Private;
    Private *d;
};

#endif // NTPCONTROLLER_H
